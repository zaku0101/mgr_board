#include "ADS1148.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "global.h"
#include "hw_config.c"

static adc_t adcs[] = {
    {
    .spi = &spis[0],
    .ss_gpio = ADC0_CS,
    .DRDY_PIN = ADC0_DRDY,
    .START_PIN = ADC0_START
    },
    {
    .spi = &spis[0],
    .ss_gpio = ADC1_CS,
    .DRDY_PIN = ADC1_DRDY,
    .START_PIN = ADC1_START
    }
}

void write_register(adc_t * adc, uint8_t reg, uint8_t data_byte) {
    uint8_t buf[4];
    buf[0] = (WREG << 4) | reg;  // shift the register address to the left by 3 bits
    buf[1] = 0x01;
    buf[2] = data_byte;

    cs_select(adc.ss_gpio);
    spi_write_blocking(adc->spi, buf, 4);
    cs_deselect(adc.ss_gpio);
    sleep_ms(10);
}

uint8_t read_register(adc_t * adc, uint8_t reg, uint8_t * ret_buff, uint16_t len) {
    uint8_t ret_val;
    uint8_t buf[2];
    buf[0] = (RREG << 4) | reg;  ;
    buf[1] = 0x01;
    cs_select(adc.ss_gpio);
    spi_write_blocking(adc->spi, buf, 2);
    spi_read_blocking(adc->spi, 0, ret_buff, len);
    cs_deselect(adc.ss_gpio);
    sleep_ms(10);
}

void adcs_init(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        write_register(&adcs[i], MUX0, 0x01);
        write_register(&adcs[i], VBIAS, 0x00);
        write_register(&adcs[i], MUX1, 0x02); //TODO what to set this to?
        write_register(&adcs[i], SYS0, 0x00); // default sampling rate
        write_register(&adcs[i], IDAC0, 0x08); // set DRDY pin as data ready active low
    }
}