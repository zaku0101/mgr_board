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

//static uint8_t dac_val[] = {0x8c, 0x8c, 0x8c, 0x99, 0x99, 0x99}; //normal dac values

static uint8_t dac_val[] = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A}; //reduced dac values for debugging

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
    //init_sd_card();

    setup_dac_spi();
    init_dac(spi1);
    set_dac(spi1, dac_val);

    //init_pwm();
    //setup_pwm_settings();
    //
    //config_spi_gpios();
    //setup_adc_spi();

    //adcs_start(adcs);
    //adcs_reset(adcs);
    //sleep_ms(1);
    //adcs_init(adcs);


    while (true) {
        //printf("Start of main loop!\n");
        printf("DAC sanity check\n");
        uint8_t buf[3];
        read_register(spi1, 0x05, buf, 0x03);
        printf("DAC register VREF: 0x%02X, 0x%02X, 0x%02X\n", buf[0], buf[1], buf[2]);
        
        write_register(spi1, 0x05, 0x00);

        read_register(spi1, 0x05, buf, 0x03);
        printf("DAC register VREF: 0x%02X, 0x%02X, 0x%02X\n", buf[0], buf[1], buf[2]);
        // read_register(spi1, PDC_CTRL_REG, buf, 0x02);
        // printf("DAC register pdc: 0x%02X, 0x%02X\n", buf[0], buf[1]);
        // read_register(spi1, GAIN_CTRL_REG, buf, 0x02);
        // printf("DAC register gain ctrl: 0x%02X, 0x%02X\n", buf[0], buf[1]);

        // for(uint8_t i = 0x00; i < 0x07; i++){
        //     read_register(spi1, adc_input_channels[i], buf, 0x02);
        //     printf("ADC register %d, value: 0x%02X, 0x%02X\n", i, buf[0], buf[1]);
        // }


        //read_adc_data(adcs, adc_input_channels, adc0_data_buff, adc1_data_buff);
       //write_data(filename, adc0_data_buff, adc1_data_buff, &counter);
        //set_dac(spi1, dac_val);
        //memset(adc0_data_buff, 0, sizeof(adc0_data_buff));
        //memset(adc1_data_buff, 0, sizeof(adc1_data_buff));
        //printf("End of main loop\n");
        sleep_ms(1000);
    }
       
    return 0;
}
