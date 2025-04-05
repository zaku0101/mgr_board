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
    //init_sd_card();

    setup_dac_spi();
    init_dac(spi1);
    set_dac(spi1);

    init_pwm();
    setup_pwm_settings();

    config_spi_gpios();
    // adcs_start(adcs);

    gpio_put(ADC0_START, 1);
    sleep_ms(5);
    gpio_put(ADC0_START, 0);


    setup_adc_spi();
    
    sleep_ms(16);
    adcs_init(adcs);

    while (true) {
        uint8_t buf[3];
        //adcs_start(adcs);

        gpio_put(ADC0_START, 1);
        sleep_ms(5);
        gpio_put(ADC0_START, 0);

        sleep_ms(1);
        write_reg(adcs[0], MUX1, 0x11); //Set for full scale external reference voltage 0-5V MOZE
        sleep_ms(1000);
        // adcs_start(adcs);

        gpio_put(ADC0_START, 1);
        sleep_ms(5);
        gpio_put(ADC0_START, 0);
    

        sleep_ms(1);
        read_reg(adcs[0], MUX1, buf, 3);
        sleep_ms(1000);
        printf("Read:%02X, %02X, %02X\n", buf[0], buf[1], buf[2]);

        read_adc_data(adcs, adc_input_channels, adc0_data_buff, adc1_data_buff);

        for (int i = 0; i < NUMBER_OF_ADC_CHANNELS; i++) {
            printf("buffer");
            printf("ADC0 channel %d: %0d\n", i, adc0_data_buff[i]);
        }
        //write_data(filename, adc0_data_buff, adc1_data_buff, &counter);

//        memset(adc0_data_buff, 0, NUMBER_OF_ADC_CHANNELS*sizeof(uint16_t));
//        memset(adc1_data_buff, 0, NUMBER_OF_ADC_CHANNELS*sizeof(uint16_t));
//
    }
       
    return 0;
}
