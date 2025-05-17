#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define ADC_VREF 3.3f
#define ADC_MAX 4095
#define ADC_CHANNEL 0
#define ADC_PIN 26
#define VOLTAGE_DIVIDER_CONST 1
#define MUX_CHANNELS 12

#define MUX_S0 14
#define MUX_S1 15
#define MUX_S2 16
#define MUX_S3 18

void init_mux();
void set_mux_channel(uint8_t channel);
float read_mux_voltage(uint8_t channel);
void read_all_mux_channels_to_buff(float *buffer);