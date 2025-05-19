#include <stdio.h>
#include "ADS1148.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "global.h"
#include "hardware/gpio.h"

volatile bool adc0_ready = false;
volatile bool adc1_ready = false;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == ADC0_DRDY) {
        adc0_ready = true;
    } else if (gpio == ADC1_DRDY) {
        adc1_ready = true;
    }
}

void setup_adc_spi() {
    spi_init(spi0, 205 * 1000);
    gpio_set_function(SPI0_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK, GPIO_FUNC_SPI);

    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
}

void send_command(adc_t adc, uint8_t command) {
    gpio_put(adc.ss_gpio, 0);
    spi_write_blocking(adc.spi, &command, 1);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(2);
}

void write_reg(adc_t adc, uint8_t reg, uint8_t data_byte) {
    uint8_t buf[4];
    buf[0] = (WREG << 4) | reg;  // shift the register address to the left by 3 bits
    buf[1] = 0x01;
    buf[2] = data_byte;

    gpio_put(ADC0_START, 1);
    sleep_ms(5);
    gpio_put(ADC0_START, 0);


    gpio_put(adc.ss_gpio, 0);
    spi_write_blocking(adc.spi, buf, 3);
    gpio_put(adc.ss_gpio, 1);
    //sleep_ms(2);
}

uint8_t read_reg(adc_t adc, uint8_t reg, uint8_t * ret_buff, uint16_t len) {
    uint8_t ret_val;
    uint8_t buf[2];
    buf[0] = (RREG << 4) | reg;  ;
    buf[1] = 0x01;

    gpio_put(ADC0_START, 1);
    sleep_ms(5);
    gpio_put(ADC0_START, 0);

    gpio_put(adc.ss_gpio, 0);
    spi_write_read_blocking(adc.spi, buf, ret_buff, len);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(2);
}

void adcs_init(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        gpio_put(ADC0_START, 1);
        sleep_ms(5);
        //gpio_put(ADC0_START, 0);

        gpio_put(adcs[i].ss_gpio, 0);
        sleep_ms(1);
        
        uint8_t cmd = RESET;
        spi_write_blocking(adcs[i].spi, &cmd, 1);
        sleep_ms(3);
        cmd = SDATAC;
        spi_write_blocking(adcs[i].spi, &cmd, 1);

        write_reg(adcs[i], MUX1, 0xA5); //Set for full scale external reference voltage 0-5V MOZE

        cmd = SYNC;
        spi_write_blocking(adcs[i].spi, &cmd, 1);
        
        gpio_put(adcs[i].ss_gpio, 1); // set DRDY pin as data ready active low
    }
}

void config_spi_gpios(){
    gpio_init(ADC0_DRDY);
    gpio_set_dir(ADC0_DRDY, GPIO_IN);
    gpio_pull_up(ADC0_DRDY);

    gpio_init(ADC1_DRDY);
    gpio_set_dir(ADC1_DRDY, GPIO_IN);
    gpio_pull_up(ADC1_DRDY);
    
    gpio_init(ADC0_START);
    gpio_set_dir(ADC0_START, GPIO_OUT);
    gpio_pull_down(ADC0_START);
    
    gpio_init(ADC1_START);
    gpio_set_dir(ADC1_START, GPIO_OUT);
    gpio_pull_down(ADC1_START);

    gpio_init(ADC0_CS);
    gpio_set_dir(ADC0_CS, GPIO_OUT);

    gpio_init(ADC1_CS);
    gpio_set_dir(ADC1_CS, GPIO_OUT);

    gpio_put(ADC0_CS, 1);
    gpio_put(ADC1_CS, 1);
}

void adcs_start(adc_t * adcs) {
    sleep_ms(16);
    for(int i = 0; i < 2; i++){
        gpio_put(adcs[i].START_PIN, 1);
        sleep_ms(5);
        gpio_put(adcs[i].START_PIN, 0);
    }
}

void adcs_reset(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        send_command(adcs[i], RESET);
        sleep_ms(1);
    }
}

void set_measured_channel(adc_t adc, uint8_t channel) {
    write_reg(adc, MUX0, channel);
}

uint16_t read_data(adc_t adc, uint8_t command) {
    uint8_t ret_buff[2];
    //uint8_t buff[] = {command};
    gpio_put(adc.ss_gpio, 0);
    //spi_write_read_blocking(adc.spi, buff, ret_buff, 3);
    send_command(adc, command);
    spi_read_blocking(adc.spi, 0, ret_buff, 2);
    gpio_put(adc.ss_gpio, 1);
    //printf("ADC data: %x %x \n", ret_buff[0], ret_buff[1]);
    return ((uint16_t)ret_buff[0] << 8) | ret_buff[1];
}

float convert_adc_data_to_real_value(uint16_t adc_data) {
    float ret_val;
    ret_val = (adc_data * 5)/65536;
    //TODO przeliczanie na wartość prądu.
    return ret_val; 
}

//MEAS AMPLIFIER HAS 20V/V amplification

void read_adc_data(adc_t * adcs, uint8_t * command_table, int16_t * adc0_meas_buff, int16_t * adc1_meas_buff) {
    for(int i = 0; i < 1; i++){
        for(int j = 0; j < NUMBER_OF_ADC_CHANNELS; j++){
            gpio_put(ADC0_START, 1);
            sleep_ms(5);
            gpio_put(ADC0_START, 0);
            gpio_put(adcs[i].ss_gpio, 0);
            send_command(adcs[i], command_table[j]);
            // send_command(adcs[i], SYNC);
            gpio_put(adcs[i].ss_gpio, 1);

            if(i == 0){
                while(gpio_get(ADC0_DRDY));
                adc0_meas_buff[j] = read_data(adcs[i], RDATA);
            }else if(i == 1){
                while(gpio_get(ADC1_DRDY));
                adc1_meas_buff[j] = read_data(adcs[i], RDATA);
            }
        }
    }

    sleep_ms(1000);
}


