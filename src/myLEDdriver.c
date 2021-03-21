/**
 * myLEDdriver.c
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#include <stdint.h>
#include "shell.h"
#include "devinio.h"
#include "myLEDdriver.h"

int led_green_fgetc(char *minor) {
    //TODO
    return 1; // value from checking LED
}

int led_green_fputc(char *minor) {
    // TODO
    return E_SUCCESS;
}

int led_orange_fgetc(char *minor) {
    //TODO
    return 1; // value from checking LED
}

int led_orange_fputc(char *minor) {
    // TODO
    return E_SUCCESS;
}

int led_yellow_fgetc(char *minor) {
    //TODO
    return 1; // value from checking LED
}

int led_yellow_fputc(char *minor) {
    // TODO
    return E_SUCCESS;
}

int led_blue_fgetc(char *minor) {
    //TODO
    return 1; // value from checking LED
}

int led_blue_fputc(char *minor) {
    // TODO
    return E_SUCCESS;
}

Device led_yellow = {
    .fgetc = &led_yellow_fgetc,
    .fputc = &led_yellow_fputc
};

Device led_blue = {
    .fgetc = &led_blue_fgetc,
    .fputc = &led_blue_fputc
};

Device led_orange = {
    .fgetc = &led_orange_fgetc,
    .fputc = &led_orange_fputc
};

Device led_green = {
    .fgetc = &led_green_fgetc,
    .fputc = &led_green_fputc
};