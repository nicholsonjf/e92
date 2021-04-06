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
#include "pcb.h"
#include "devinutils.h"
#include <stdint.h>
#include <string.h>

Device LEDGreen;
Device LEDYellow;
Device LEDBlue;
Device LEDOrange;

int ledfgetc(file_descriptor *fd)
{
    return E_NOT_SUPPORTED;
}

int ledfputc(file_descriptor *fd, char *bufp, int buflen)
{
	char yellow[] = "/dev/ledy";
    if (strcmp((char*)&(currentPCB->streams)[*fd].pathname,  &yellow[0]) == 0) {
        ledYellowOn();
    }
	char orange[] = "/dev/ledo";
    if (strcmp((char*)&(currentPCB->streams)[*fd].pathname,  &orange[0]) == 0) {
        ledOrangeOn();
    }
	char blue[] = "/dev/ledb";
    if (strcmp((char*)&(currentPCB->streams)[*fd].pathname,  &blue[0]) == 0) {
        ledBlueOn();
    }
	char green[] = "/dev/ledg";
    if (strcmp((char*)&(currentPCB->streams)[*fd].pathname,  &green[0]) == 0) {
        ledGreenOn();
    }
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
    // Get an available Stream or return an error
    int get_stream_status = get_available_stream(fd);
    if (get_stream_status != E_SUCCESS)
    {
        return get_stream_status;
    }
    return E_SUCCESS;
}

int ledfcreate(char *pathname) {
    return E_NOT_SUPPORTED;
}

int initLED(void)
{
    /* Initialize all of the LEDs */
    ledInitAll();
    // Assign the green led functions
    LEDGreen.fgetc = ledfgetc;
    LEDGreen.fputc = ledfputc;
    LEDGreen.fclose = ledfclose;
    LEDGreen.fdelete = ledfdelete;
    LEDGreen.fcreate = ledfcreate;
    LEDGreen.fopen = ledfopen;
    // Assign the green led functions
    LEDYellow.fgetc = ledfgetc;
    LEDYellow.fputc = ledfputc;
    LEDYellow.fclose = ledfclose;
    LEDYellow.fdelete = ledfdelete;
    LEDYellow.fcreate = ledfcreate;
    LEDYellow.fopen = ledfopen;
    // Assign the yellow led functions    // Assign the green led functions
    LEDBlue.fgetc = ledfgetc;
    LEDBlue.fputc = ledfputc;
    LEDBlue.fclose = ledfclose;
    LEDBlue.fdelete = ledfdelete;
    LEDBlue.fcreate = ledfcreate;
    LEDBlue.fopen = ledfopen;
    // Assign the yellow led functions    // Assign the green led functions
    LEDOrange.fgetc = ledfgetc;
    LEDOrange.fputc = ledfputc;
    LEDOrange.fclose = ledfclose;
    LEDOrange.fdelete = ledfdelete;
    LEDOrange.fcreate = ledfcreate;
    LEDOrange.fopen = ledfopen;
    // Assign the yellow led functions
    return E_SUCCESS;
}
