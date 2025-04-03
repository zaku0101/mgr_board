#include <stdio.h>
#include "MCP48CV.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

uint8_t adc_reg_addr[DAC_CHANNEL_COUNT] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void cs_select(int cs_pin) {
    gpio_put(cs_pin, 0);
}

void cs_deselect(int cs_pin) {
    gpio_put(cs_pin, 1);
}

void setup_dac_spi() {
    spi_init(spi1, 25000 * 1000);

    gpio_set_function(SPI1_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_SCK, GPIO_FUNC_SPI);
    
    gpio_init(DAC_CS);
    gpio_set_dir(DAC_CS, GPIO_OUT);
    cs_deselect(DAC_CS);

    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    uint8_t ret_buf[4];
}

void write_register(spi_inst_t *spi, uint8_t reg, uint8_t data) {
    uint8_t buf[3];
    buf[0] = reg << 3;  // shift the register address to the left by 3 bits
    buf[1] = 0x00;
    buf[2] = data;
    cs_select(DAC_CS);
    spi_write_blocking(spi, buf, 3);
    spi_write_blocking(spi, buf, 3);
    cs_deselect(DAC_CS);
    sleep_ms(2);
}
void read_register(spi_inst_t *spi, uint8_t reg, uint8_t * ret_buff, uint16_t len) {
    reg = (reg << 3) | 0x06;
    uint8_t buf[len];
    cs_select(DAC_CS);
    for(int i = 0; i < len; i++){
        spi_read_blocking(spi, reg, &ret_buff[i], 1); 
    }
    cs_deselect(DAC_CS);
    sleep_ms(2);
}

void init_dac(spi_inst_t *spi) {
    printf("Initializing DAC...\n");
    write_register(spi, VREF_CTRL_REG, 0x00);
    write_register(spi, PDC_CTRL_REG, 0x00);
    write_register(spi, GAIN_CTRL_REG, 0x00);
}

void set_dac(spi_inst_t *spi, uint8_t * values) {
    printf("Setting DAC values...\n");
    for(int i =0; i < DAC_CHANNELS; i++) {      
        write_register(spi, adc_reg_addr[i], values[i]);
        sleep_ms(10);
    }
}
