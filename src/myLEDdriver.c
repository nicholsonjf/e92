/**
 * myLEDdriver.c
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#include "myLEDdriver.h"
#include "devinio.h"
#include "led.h"
#include "utils.h"
#include <stdint.h>

int ledfgetc(file_descriptor *fd)
{
    return E_NOT_SUPPORTED;
}

int ledfputc(char c, file_descriptor *fd)
{
    return E_SUCCESS;
}

int ledfclose(file_descriptor *fd)
{
    return E_SUCCESS;
}

int ledfdelete(char *pathname) {
    return E_NOT_SUPPORTED;
}

int ledfopen(char *pathname, file_descriptor *fd)
{
    return E_NOT_SUPPORTED;
}

int ledfcreate(char *pathname, file_descriptor *fd) {
    return E_NOT_SUPPORTED;
}

int initLED(void)
{
    /* Initialize all of the LEDs */
    ledInitAll();
    // Define the struct Devices
    Device *leds[] = {LEDYellow, LEDGreen, LEDOrange, LEDBlue};
    for (int i=0; i<sizeof(leds)/sizeof(leds[0]); i++) {
        leds[i]->fgetc = ledfgetc;
        leds[i]->fputc = ledfputc;
        leds[i]->fclose = ledfclose;
        leds[i]->fdelete = ledfdelete;
        leds[i]->fcreate = ledfcreate;
        leds[i]->fopen = ledfopen;
    }
    return E_SUCCESS;
}