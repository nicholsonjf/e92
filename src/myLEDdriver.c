/**
 * myLEDdriver.c
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#include <stdint.h>
#include "shell.h"


static struct driver led_green;
static struct driver led_yellow;
static struct driver led_blue;
static struct driver led_orange;

int led_green_fgetc(void) {
    //TODO
    return; // value from checking LED
}

int led_green_fputc(void) {
    // TODO
    return E_SUCCESS
}

int led_orange_fgetc(void) {
    //TODO
    return; // value from checking LED
}

int led_orange_fputc(void) {
    // TODO
    return E_SUCCESS
}

int led_yellow_fgetc(void) {
    //TODO
    return; // value from checking LED
}

int led_yellow_fputc(void) {
    // TODO
    return E_SUCCESS
}

int led_blue_fgetc(void) {
    //TODO
    return; // value from checking LED
}

int led_blue_fputc(void) {
    // TODO
    return E_SUCCESS
}

int initLED(void) {
    led_green.fgetc = led_green_fgetc;
    led_green.fputc = led_green_fgetc;
    led_orange.fgetc = led_orange_fgetc;
    led_orange.putc = led_orange_fputc;
    led_yellow.fgetc = led_yellow_fgetc;
    led_yellow.fputc = led_yellow_fputc;
    led_blue.fgetc = led_blue_fgetc;
    led_blue.fputc = led_blue_fputc;
}