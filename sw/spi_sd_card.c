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
#include "hw_config.c"
#include "global.h"


void handle_error(const char *message, FRESULT error_code) {
    printf("Error: %s (%d)\n", message, error_code);
    exit(EXIT_FAILURE);
}

void write_to_file(const char *filename, const char *message) {
    FIL file;
    FRESULT result;

    // Open the file in append mode
    result = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
    if (result != FR_OK && result != FR_EXIST) {
        handle_error("Failed to open file", result);
    }

    // Write the message to the file
    if (f_printf(&file, "%s", message) < 0) {
        printf("Failed to write to file\n");
    }

    // Close the file
    result = f_close(&file);
    if (result != FR_OK) {
        handle_error("Failed to close file", result);
    }
}

void write_header(const char *filename) {
    write_to_file(filename, "Timestamp,");
    for (int i = 1; i <= NUM_ADC_CHANNELS / 2; i++) {
        char column_name[20]; // Buffer to hold the combined string
        sprintf(column_name, "V_D%d,I_D%d", i, i);
        write_to_file(filename, column_name);
        if (i < NUM_ADC_CHANNELS / 2) {
            write_to_file(filename, ",");
        }
    }
    write_to_file(filename, "\n");
}

void init_sd_card() {

    printf("Hello, world!\n");

    // Initialize the SD card and mount the filesystem
    sd_card_t *sd_card = sd_get_by_num(0);
    FRESULT mount_result = f_mount(&sd_card->fatfs, sd_card->pcName, 1);
    if (mount_result != FR_OK) {
        handle_error("Failed to mount filesystem", mount_result);
    }

    // Write a message to a file
    const char *filename = FILENAME;
    write_header(filename);

    // Unmount the filesystem
    f_unmount(sd_card->pcName);
}