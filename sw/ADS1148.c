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
    spi_init(spi0, 1000 * 1000);
    gpio_set_function(SPI0_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK, GPIO_FUNC_SPI);

    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
}

static void send_command(adc_t adc, uint8_t command) {
    gpio_put(adc.ss_gpio, 0);
    sleep_us(2);
    spi_write_blocking(adc.spi, &command, 1);
    sleep_us(2);
    gpio_put(adc.ss_gpio, 1);
    sleep_us(5);
}

static void write_reg(adc_t adc, uint8_t reg, uint8_t data_byte) {
    uint8_t buf[3] = {0};
    buf[0] = (WREG << 4) | reg;  // shift the register address to the left by 3 bits
    buf[1] = 0; // becouse of len of bytes = 1, thats why 0;
    buf[2] = data_byte;
    gpio_put(adc.ss_gpio, 0);
    sleep_us(2);
    spi_write_blocking(adc.spi, buf, sizeof(buf));
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(2);
}

static uint8_t read_reg(adc_t adc, uint8_t reg, uint8_t* ret_buff) {
    uint8_t ret_val;
    uint8_t buf[2] = {0};
    buf[0] = (RREG << 4) | reg;
    
    gpio_put(adc.ss_gpio, 0);
    sleep_us(2);
    spi_write_blocking(adc.spi, buf, sizeof(buf));
    spi_read_blocking(adc.spi, NOP, ret_buff, 1);
    sleep_us(2); 
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(2);
}

void adcs_init(adc_t * adcs) {

    for(int i = 0; i < 2; i++){
        send_command(adcs[i], RESET);
        sleep_ms(2);
        send_command(adcs[i], SDATAC);
    }
}

void toogle_start(uint8_t start_pin) {
    gpio_put(start_pin, 1);
    sleep_ms(2);
    gpio_put(start_pin, 0);
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
    sleep_ms(16);
    gpio_put(ADC0_START, 1);
    gpio_put(ADC1_START, 1);
}

// void adcs_start(adc_t * adcs) {
//     sleep_ms(16);
//     for(int i = 0; i < 2; i++){
//         gpio_put(adcs[i].START_PIN, 1);
//         sleep_ms(5);
//         gpio_put(adcs[i].START_PIN, 0);
//     }
// }

void adcs_reset(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        send_command(adcs[i], RESET);
        sleep_ms(1);
    }
}

void set_measured_channel(adc_t adc, uint8_t channel) {
    write_reg(adc, MUX0, channel);
}

uint16_t read_data(adc_t adc, uint8_t len) {
    uint8_t rdata_command = RDATA;
    uint8_t ret_buff[2];
    gpio_put(adc.ss_gpio, 0);
    sleep_us(2);
    spi_write_blocking(adc.spi, &rdata_command, 1);
    spi_read_blocking(adc.spi, NOP, &ret_buff, sizeof(ret_buff));
    sleep_us(2);
    gpio_put(adc.ss_gpio, 1);
    sleep_us(5);
    
    //printf("ADC data: %x %x %x\n", ret_buff[0], ret_buff[1],ret_buff[2]);
    return ((uint16_t)ret_buff[1] << 8) | ret_buff[2];
}

float convert_adc_data_to_real_value(uint16_t adc_data) {
    return ((int16_t)adc_data) * 5.0f / 32768.0f / 20.0f;
}

//MEAS AMPLIFIER HAS 20V/V amplification

void read_adc_data(adc_t * adcs, uint8_t * command_table, int16_t * adc0_meas_buff, int16_t * adc1_meas_buff) {
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < NUMBER_OF_ADC_CHANNELS; j++){
            uint8_t cmd = SYNC;
            uint8_t recv_command;
            
            write_reg(adcs[i], MUX0, command_table[j]);
            read_reg(adcs[i], MUX0, &recv_command);
            printf("MUX0: %x\n", recv_command);
            send_command(adcs[i], SYNC);
            while(gpio_get(adcs[i].DRDY_PIN) == 0){
                tight_loop_contents(); 
            }
            adc0_meas_buff[j] = read_data(adcs[i], RDATA);
            sleep_ms(200);
        }
    }
}


