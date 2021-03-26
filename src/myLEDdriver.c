/**
 * myLEDdriver.c
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#include <stdint.h>
#include "devinio.h"
#include "led.h"
#include "myLEDdriver.h"
#include "utils.h"

int fgetc(void)
{
    return E_SUCCESS;
}

int fputc(void)
{
    return E_SUCCESS;
}

int fclose(void)
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
        leds[i]->fgetc = fgetc;
        leds[i]->fputc = fputc;
        leds[i]->fclose = fclose;
    }
    return E_SUCCESS;
}