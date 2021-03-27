/**
 * devinit.c
 * Device initialization
 * 
 * Author: James Nicholson
 */

#include "devinio.h"
#include <string.h>
#include "SDHC_FAT32_Files.h"
#include "pcb.h"
#include "my-malloc.h"
#include "myFAT32driver.h"
#include "myLEDdriver.h"
#include "myPBdriver.h"
#include "utils.h"

#define NUMBER_OF_DEVICES 7

typedef struct device_p
{
    char *pathname;
    Device *device;
} device_p;

device_p *devices;

int initDevices(void)
{
    int initLED_status = initLED();
    if (initLED_status != E_SUCCESS)
    {
        return initLED_status;
    }
    int initPB_status = initPB();
    if (initPB_status != E_SUCCESS)
    {
        return initPB_status;
    }
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS)
    {
        return initFAT_status;
    }
    devices = myMalloc(NUMBER_OF_DEVICES * sizeof(device_p));
    device_p *initializedDevices = {
        {"/", FAT32},
        {"/dev/pushbutton/sw1", SW1},
        {"/dev/pushbutton/sw2", SW2},
        {"/dev/led/yellow", LEDYellow},
        {"/dev/led/orange", LEDOrange},
        {"/dev/led/blue", LEDBlue},
        {"/dev/led/green", LEDGreen}};
    devices = initializedDevices;
    return E_SUCCESS;
}