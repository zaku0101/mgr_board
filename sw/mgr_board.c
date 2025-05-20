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
#include "mux.h"

int pico_adc_val;
float meas_data[MEAS_DATA_BUFF_SIZE];
int counter = 0;

float voltage_buffer[MUX_CHANNELS];

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

int16_t adc0_data_buff[NUMBER_OF_ADC_CHANNELS];
int16_t adc1_data_buff[NUMBER_OF_ADC_CHANNELS];

int main() {
    stdio_init_all();
    init_mux();
    //init_sd_card();

    setup_dac_spi();
    init_dac(spi1);
    set_dac(spi1);

    init_pwm();
    setup_pwm_settings();

    config_spi_gpios();
    setup_adc_spi();
    
    adcs_init(adcs);

    while (true) {
        printf("\n\n!!!NEW MEASUREMENT!!!\n\n");
        uint16_t value=0;
        uint8_t adc0_reg_val = 0;

        read_adc_data(adcs, adc_input_channels, adc0_data_buff, adc1_data_buff);

        for(int i =0; i<NUMBER_OF_ADC_CHANNELS;i++){
            // printf("ADC0:%d %04X\n",i ,adc0_data_buff[i]);
            // printf("ADC1:%d %04X\n",i ,adc1_data_buff[i]);
            printf("ADC0:%d %f\n",i ,convert_adc_data_to_real_value(adc0_data_buff[i]));
            printf("ADC1:%d %f\n",i ,convert_adc_data_to_real_value(adc1_data_buff[i]));
        }

        sleep_ms(1000);
    }
       
    return 0;
}
