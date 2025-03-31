

#include <stdio.h>
#include "ADS1148.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "global.h"


volatile bool adc0_ready = false;
volatile bool adc1_ready = false;

uint32_t last_interrupt_time_adc0 = 0;
uint32_t last_interrupt_time_adc1 = 0;

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == ADC0_DRDY) {
        if (current_time - last_interrupt_time_adc0 > 10) { // 10 ms debounce
            adc0_ready = true;
            last_interrupt_time_adc0 = current_time;
        }
    } else if (gpio == ADC1_DRDY) {
        if (current_time - last_interrupt_time_adc1 > 10) { // 10 ms debounce
            adc1_ready = true;
            last_interrupt_time_adc1 = current_time;
        }
    }
}

void setup_adc_spi() {
    spi_init(spi0, 12500 * 1000);
    gpio_set_function(SPI1_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK, GPIO_FUNC_SPI);

    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void send_command(adc_t adc, uint8_t command) {
    gpio_put(adc.ss_gpio, 0);
    sleep_ms(1);
    spi_write_blocking(adc.spi, &command, 1);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(10);
}

void write_reg(adc_t adc, uint8_t reg, uint8_t data_byte) {
    uint8_t buf[4];
    buf[0] = (WREG << 4) | reg;  // shift the register address to the left by 3 bits
    buf[1] = 0x01;
    buf[2] = data_byte;

    gpio_put(adc.ss_gpio, 0);
    sleep_ms(1);
    spi_write_blocking(adc.spi, buf, 4);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(10);
}

uint8_t read_reg(adc_t adc, uint8_t reg, uint8_t * ret_buff, uint16_t len) {
    uint8_t ret_val;
    uint8_t buf[2];
    buf[0] = (RREG << 4) | reg;  ;
    buf[1] = 0x01;
    
    gpio_put(adc.ss_gpio, 0);
    sleep_ms(1);
    spi_write_blocking(adc.spi, buf, 2);
    spi_read_blocking(adc.spi, 0, ret_buff, len);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(10);
}

void adcs_init(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        gpio_put(adcs[i].ss_gpio, 0);
        sleep_ms(1);
        write_reg(adcs[i], VBIAS, 0x00);// nothing to set here
        write_reg(adcs[i], MUX1, 0x02); //Set for full scale external reference voltage 0-5V
        write_reg(adcs[i], SYS0, 0x00); // default sampling rate 5 SPS, imo its enough for DC voltage measurement
        write_reg(adcs[i], GPIOCFG, 0x40); //set ain6 as gpio output grounded for negative reference
        write_reg(adcs[i], IDAC0, 0x08);
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
    gpio_put(ADC0_CS, 1);

    gpio_init(ADC1_CS);
    gpio_set_dir(ADC1_CS, GPIO_OUT);
    gpio_put(ADC1_CS, 1);

    gpio_set_irq_enabled_with_callback(ADC0_DRDY, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ADC1_DRDY, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void adcs_start(adc_t * adcs) {
    sleep_ms(16);
    for(int i = 0; i < 2; i++){
        gpio_put(adcs[i].START_PIN, 1);
        sleep_ms(2);
        gpio_put(adcs[i].START_PIN, 0);
    }
}

void adcs_reset(adc_t * adcs) {
    for(int i = 0; i < 2; i++){
        gpio_put(adcs[i].ss_gpio, 0);
        sleep_ms(1);
        send_command(adcs[i], RESET);
        gpio_put(adcs[i].ss_gpio, 1);
        sleep_ms(1);
    }
    sleep_ms(1);
}


void set_measured_channel(adc_t adc, uint8_t channel) {
    write_reg(adc, MUX0, channel);
}

uint16_t read_data(adc_t adc, uint8_t command) {
    uint8_t ret_buff[2];
    uint8_t buff[] = {command};
    gpio_put(adc.ss_gpio, 0);
    sleep_ms(1);
    spi_write_read_blocking(adc.spi, buff, ret_buff, 3);
    gpio_put(adc.ss_gpio, 1);
    sleep_ms(10);
    return (ret_buff[0] << 8) | ret_buff[1];
}

uint16_t convert_adc_data_to_real_value(uint16_t adc_data) {
    uint16_t ret_val;
    ret_val = (adc_data * 5) / 32768;
    //TODO przeliczanie na wartość prądu.
    return ret_val; 
}

void read_adc_data(adc_t * adcs, uint8_t * command_table, uint16_t * adc0_meas_buff, uint16_t * adc1_meas_buff) {
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < sizeof(adc0_meas_buff) / sizeof(uint16_t); j++){
            send_command(adcs[i], SDATAC);
            sleep_ms(1);
            send_command(adcs[i], command_table[j]);
            send_command(adcs[i], SYNC);
            sleep_ms(1);

            gpio_put(adcs[i].ss_gpio, 1);
            if(i == 0){
                while(!adc0_ready){
                    tight_loop_contents();
                }
                printf("ADC0 ready\n");
                adc0_ready = false; // Reset the flag
                adc0_meas_buff[j] = convert_adc_data_to_real_value(read_data(adcs[i], RDATA));
                sleep_ms(1);
            }else{
                while(!adc1_ready){
                    tight_loop_contents();
                }
                printf("ADC1 ready\n");
                adc1_ready = false; // Reset the flag
                adc1_meas_buff[j] = convert_adc_data_to_real_value(read_data(adcs[i], RDATA));
                sleep_ms(1);
            }
        }
    }
}


