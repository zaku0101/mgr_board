#include "MCP48CV.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

uint8_t adc_reg_addr[DAC_CHANNEL_COUNT] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //set half to 0x9b and half to 0x7c

void cs_select(int cs_pin) {
    gpio_put(cs_pin, 0);
}

void cs_deselect(int cs_pin) {
    gpio_put(cs_pin, 1);
}

void write_register(spi_inst_t * spi, uint8_t reg, uint8_t data) {
    uint8_t buf[3];
    buf[0] = reg << 3;  // shift the register address to the left by 3 bits
    buf[1] = 0x00;
    buf[2] = data;
    cs_select(DAC_CS);
    spi_write_blocking(spi, buf, 3);
    cs_deselect(DAC_CS);
    sleep_ms(10);
}

void init_dac(spi_inst_t * spi) {
    write_register(spi, VREF_CTRL_REG, 0x00);
    write_register(spi, PDC_CTRL_REG, 0x00);
    //write_register(spi, GAIN_CTRL_REG, 0x00);
}

void set_dac(spi_inst_t * spi, uint8_t * channels, uint8_t * values) {
    for(int i =0; i < DAC_CHANNELS; i++) {
        write_register(spi, channels[i], values[i]);
    }
}
