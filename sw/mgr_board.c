#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "sd_card.h"
#include "ff.h"
#include "spi_sd_card.c"
#include "MCP48CV.h"
#include "ADS1148.h"
#include "global.h"
#include "pwm.h"

int pico_adc_val;
float meas_data[MEAS_DATA_BUFF_SIZE];
int counter = 0;
static uint8_t dac_val[] = {0x9b, 0x9b, 0x9b, 0x7c, 0x7c, 0x7c};
const char *filename = FILENAME;
static uint8_t adc_input_channels[] = {0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x3E};

static adc_t adcs[] = {
    {
    .spi = spi0,
    .ss_gpio = ADC0_CS,
    .DRDY_PIN = ADC0_DRDY,
    .START_PIN = ADC0_START
    },
    {
    .spi = spi0,
    .ss_gpio = ADC1_CS,
    .DRDY_PIN = ADC1_DRDY,
    .START_PIN = ADC1_START
    }
};

uint16_t adc0_data_buff[NUMBER_OF_ADC_CHANNELS];
uint16_t adc1_data_buff[NUMBER_OF_ADC_CHANNELS];


int main() {
    stdio_init_all();
    init_sd_card();
    init_dac(spis[0].hw_inst);
    set_dac(spis[0].hw_inst, adc_reg_addr, dac_val);

    init_pwm();
    setup_pwm_settings();

    adcs_start(adcs);
    config_spi_gpios();
    setup_adc_spi();
    adcs_reset(adcs);
    sleep_ms(1);
    adcs_init(adcs);


    while (true) {
        printf("Hello, world!\n");

        memset(adc0_data_buff, 1, sizeof(adc0_data_buff));
        memset(adc1_data_buff, 1, sizeof(adc1_data_buff));

        //read_adc_data(adcs, adc_input_channels, adc0_data_buff, adc1_data_buff);
        write_data(filename, adc0_data_buff, adc1_data_buff, &counter);

        memset(adc0_data_buff, 0, sizeof(adc0_data_buff));
        memset(adc1_data_buff, 0, sizeof(adc1_data_buff));

        sleep_ms(1000);
    }

    return 0;
}
