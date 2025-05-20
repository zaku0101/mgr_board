#pragma once

#include <stdint.h>
#include "hardware/spi.h"
#include "global.h"

#define DAC_CHANNEL_COUNT 8 // Number of DAC channels
#define VREF_CTRL_REG 0x08 // Voltage Reference Control Register, set to 0x00
#define PDC_CTRL_REG 0x09 // Power Down Control Register, set to 0x00
#define GAIN_CTRL_REG 0x1A // Gain Control Register, when vref is set to vdd register values are ignored

//#define LOW_DAC_VALUE 0x98
//#define HIGH_DAC_VALUE 0x98

#define LOW_DAC_VALUE  0x01
#define HIGH_DAC_VALUE 0x0F

extern uint8_t adc_reg_addr[DAC_CHANNEL_COUNT];

void cs_select(int cs_pin);
void cs_deselect(int cs_pin);
void write_register(spi_inst_t *spi, uint8_t reg, uint8_t data);
void init_dac(spi_inst_t *spi);
void set_dac(spi_inst_t *spi);
void setup_dac_spi();
void read_register(spi_inst_t *spi, uint8_t reg, uint8_t * ret_buff, uint16_t len);
