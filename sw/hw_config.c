#include <string.h>
#include "my_debug.h"
#include "hw_config.h"
#include "ff.h" /* Obtains integer types */
#include "diskio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
//uart
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0  
#define UART_RX_PIN 1  
//spi0 pins
#define SPI0_MISO 0
#define SPI0_MOSI 3
#define SPI0_SCK 2

//spi1 pins
#define SPI1_MISO 12
#define SPI1_MOSI 11
#define SPI1_SCK 10

#define DAC_CS 13 
#define ADC0_CS 1
#define ADC1_CS 5
#define ADC2_CS 9
#define SD_CS 17

static spi_t spis[] = {  // One for each SPI.
    {
        .hw_inst = spi0,  // SPI component
        .miso_gpio = SPI0_MISO, // GPIO number (not pin number)
        .mosi_gpio = SPI0_MOSI,
        .sck_gpio = SPI0_SCK,
        .baud_rate = 12500 * 1000,  
        //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. 
    },
    {
        .hw_inst = spi1,  // SPI component
        .miso_gpio = SPI1_MISO, // GPIO number (not pin number)
        .mosi_gpio = SPI1_MOSI,
        .sck_gpio = SPI1_SCK,
        .baud_rate = 12500 * 1000,  
        //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. 
    }
};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_cards[] = {  // One for each SD card
    {
        .pcName = "0:",   // Name used to mount device
        .spi = &spis[1],  // Pointer to the SPI driving this card
        .ss_gpio = SD_CS,    // The SPI slave select GPIO for this SD card
        .use_card_detect = false,   // Card detect
        .card_detected_true = -1  // What the GPIO read returns when a card is
                                 // present. Use -1 if there is no card detect.
    }};

/* ********************************************************************** */
size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}