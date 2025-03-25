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

int spi_adc_read_buff[NUM_SPI_ADC_CHANNELS];
int pico_adc_val;
float meas_data[MEAS_DATA_BUFF_SIZE];
uint8_t dac_val[DAC_CHANNELS] = {0x9b, 0x9b, 0x9b, 0x7c, 0x7c, 0x7c};

int main() {
    stdio_init_all();
    init_sd_card();
    init_dac(spis[0].hw_inst);
    set_dac(spis[0].hw_inst, adc_reg_addr, dac_val);

    printf("Goodbye, world!\n");

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }

    return 0;
}
