#pragma once

#include <stdint.h>
#include "hardware/spi.h"
#include "global.h"

//spi commands
#define WAKEUP 0x00
#define SLEEP 0x02
#define SYNC 0x04
#define RESET 0x06
#define NOP 0xFF
#define RDATA 0x12
#define RDATAC 0x14
#define SDATAC 0x16
#define RREG 0x02
#define WREG 0x04
#define SYSOCAL 0x60
#define SYSGCAL 0x61
#define SELFOCAL 0x62

//adc registers
#define MUX0 0x00
#define VBIAS 0x01
#define MUX1 0x02
#define SYS0 0x03
#define OFC0 0x04
#define OFC1 0x05
#define OFC2 0x06
#define FSC0 0x07
#define FSC1 0x08
#define FSC2 0x09
#define IDAC0 0x0A
#define IDAC1 0x0B
#define GPIOCFG 0x0C
#define GPIODIR 0x0D
#define GPIODAT 0x0E


typedef struct {
    spi_inst_t *spi;
    uint ss_gpio;
    uint DRDY_PIN;
    uint START_PIN;

}adc_t;

void gpio_callback(uint gpio, uint32_t events);
void send_command(adc_t adc, uint8_t command);
void config_spi_gpios();
void setup_adc_spi();
void adcs_start(adc_t * adcs);
void adcs_reset(adc_t * adcs);
void set_measured_channel(adc_t adc, uint8_t channel);
uint16_t read_data(adc_t adc, uint8_t command);
float convert_adc_data_to_real_value(uint16_t adc_data);
void adcs_init(adc_t * adcs);
uint8_t read_reg(adc_t adc, uint8_t reg, uint8_t * ret_buff, uint16_t len);
void write_reg(adc_t adc, uint8_t reg, uint8_t data_byte);
void read_adc_data(adc_t * adcs, uint8_t * command_table, int16_t * adc0_meas_buff, int16_t * adc1_meas_buff);
