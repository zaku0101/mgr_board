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

void send_command(adc_t * adc, uint8_t command) {
    cs_select(adc.ss_gpio);
    spi_write_blocking(adc->spi, &command, 1);
    cs_deselect(adc.ss_gpio);
    sleep_ms(10);
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
        write_register(&adcs[i], MUX0, 0x06); //Set default operation, positive AIN0, negative AIN6 negative reference as gpio default low.
        write_register(&adcs[i], VBIAS, 0x00);// nothing to set here
        write_register(&adcs[i], MUX1, 0x02); //Set for full scale external reference voltage 0-5V
        write_register(&adcs[i], SYS0, 0x00); // default sampling rate 5 SPS, imo its enough for DC voltage measurement
        write_register(&adcs[i], GPIOCFG, 0x40); //set ain6 as gpio output grounded for negative reference
        write_register(&adcs[i], IDAC0, 0x08); // set DRDY pin as data ready active low
    }
}

void adcs_start(adc_t * adcs) {
    sleep_ms(16);
    for(int i = 0; i < 2; i++){
        gpio_put(adcs[i].START_PIN, 1);
        gpio_put(adcs[i].START_PIN, 0);
    }
}

void adcs_reset(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        send_command(&adcs[i], RESET);
    }
    sleep_ms(1);
}

