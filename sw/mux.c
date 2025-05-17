#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "mux.h"

void init_mux() {
    // Initialize the GPIO pins for the multiplexer
    gpio_init(MUX_S0);
    gpio_set_dir(MUX_S0, GPIO_OUT);
    gpio_init(MUX_S1);
    gpio_set_dir(MUX_S1, GPIO_OUT);
    gpio_init(MUX_S2);
    gpio_set_dir(MUX_S2, GPIO_OUT);
    gpio_init(MUX_S3);
    gpio_set_dir(MUX_S3, GPIO_OUT);

    // Initialize the ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHANNEL);
}

void set_mux_channel(uint8_t channel) {
    // Set the GPIO pins to select the desired channel
    gpio_put(MUX_S0, (channel >> 0) & 0x01);
    gpio_put(MUX_S1, (channel >> 1) & 0x01);
    gpio_put(MUX_S2, (channel >> 2) & 0x01);
    gpio_put(MUX_S3, (channel >> 3) & 0x01);
}
float read_mux_voltage(uint8_t channel) {
    set_mux_channel(channel);
    sleep_ms(10); // Wait for the mux to settle
    uint16_t raw_value = adc_read();
    float voltage = (raw_value / (float)ADC_MAX) * ADC_VREF * VOLTAGE_DIVIDER_CONST;
    return voltage;
}
void read_all_mux_channels_to_buff(float *buffer) {
    for (uint8_t channel = 0; channel < MUX_CHANNELS; channel++) {
        buffer[channel] = read_mux_voltage(channel);
    }
}