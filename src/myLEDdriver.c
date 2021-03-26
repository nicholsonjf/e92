/**
 * myLEDdriver.c
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#include "myLEDdriver.h"
#include <stdint.h>
#include "devinio.h"
#include "led.h"
#include "utils.h"

int ledfgetc(void)
{
    return E_SUCCESS;
}

int ledfputc(void)
{
    return E_SUCCESS;
}

int ledfclose(void)
{
    return E_SUCCESS;
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
    }
    return E_SUCCESS;
}