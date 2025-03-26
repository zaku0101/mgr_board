#pragma once

#include <stdint.h>
#include "hardware/spi.h"
#include "global.h"

//adc control pins
#define ADC0_DRDY 6
#define ADC1_DRDY 7
#define ADC1_START 8
#define ADC0_START 9

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
    spi_t *spi;
    uint ss_gpio;
    uint DRDY_PIN;
    uint START_PIN;

}adc_t;

void adcs_init = (adc_t * adcs);
void read_register = (adc_t * adc, uint8_t reg, uint8_t addr);
void write_register = (adc_t * adc, uint8_t reg, uint8_t data);

