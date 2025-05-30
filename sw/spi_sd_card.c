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
    sd_card_t *sd_card = sd_get_by_num(0);
    FRESULT mount_result = f_mount(&sd_card->fatfs, sd_card->pcName, 1);
    if (mount_result != FR_OK) {
        handle_error("Failed to mount filesystem", mount_result);
    }

    write_to_file(filename, "Hour,");
    for (int i = 1; i <= NUMBER_OF_ADC_CHANNELS; i++) {
        char column_name[20]; // Buffer to hold the combined string
        sprintf(column_name, "ADC0_%d,ADC1_%d", i, i);
        write_to_file(filename, column_name);
        if (i < NUMBER_OF_ADC_CHANNELS) {
            write_to_file(filename, ",");
        }
    }
    write_to_file(filename, ",");
    
    for (int i = 0; i < 16; i++) {
        char voltage_column[20];
        sprintf(voltage_column, "V_MUX_%d", i);
        write_to_file(filename, voltage_column);
        if (i < 16) {
            write_to_file(filename, ",");
        }
    }
    write_to_file(filename, "\n");
    f_unmount(sd_card->pcName);
}

void write_data(const char *filename, float *adc0_buff, float *adc1_buff,float *voltage_buffer, int *counter) {
    sd_card_t *sd_card = sd_get_by_num(0);
    FRESULT mount_result = f_mount(&sd_card->fatfs, sd_card->pcName, 1);
    if (mount_result != FR_OK) {
        handle_error("Failed to mount filesystem", mount_result);
    }

    char buffer[40]; // Temporary buffer for small chunks of data

    // Write the counter value
    snprintf(buffer, sizeof(buffer), "%d", *counter);
    write_to_file(filename, buffer);

    // Append ADC data
    for (int i = 0; i < NUMBER_OF_ADC_CHANNELS; i++) {
        snprintf(buffer, sizeof(buffer), ",%f,%f", adc0_buff[i], adc1_buff[i]);
        printf("Writing data: %s\n", buffer); // Debug print
        write_to_file(filename, buffer);
    }
    for(int i =0; i < 16 ; i++){
        snprintf(buffer, sizeof(buffer), ",%f", voltage_buffer[i]);
        printf("Writing voltage data: %s\n", buffer); // Debug print
        write_to_file(filename, buffer);
    }
    // Add a newline at the end
    write_to_file(filename, "\n");

    // Increment the counter
    (*counter)++;

    f_unmount(sd_card->pcName);
}

void init_sd_card() {

    const char *filename = FILENAME;
    write_header(filename);
  
}