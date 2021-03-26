/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include <string.h>
#include "shell.h"
#include "SDHC_FAT32_Files.h"
#include "myFAT32driver.h"
#include "devinio.h"
#include "pcb.h"
#include "my-malloc.h"
#include "myFAT32driver.h"
#include "myLEDdriver.h"
#include "myPBdriver.h"


#define NUMBER_OF_DEVICES 7

typedef struct device_p
{
    char *pathname;
    Device *device;
} device_p;

device_p *devices;

int initDevIO(void)
{
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS) {
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
        {"/dev/led/green", LEDGreen}
    };
    devices = initializedDevices;
    return E_SUCCESS;
}

int get_device(char *pathname, Device *device)
{
    for (int i=0; i<NUMBER_OF_DEVICES; i++)
    {
        if (strcmp(pathname, devices[i].pathname) == 0)
        {
            device = devices[i].device;
        }
    }
    return E_DEVICE_PATH;
}

int fopen(char *pathname, file_descriptor *fd)
{
    Device *device = myMalloc(sizeof(Device));
    int get_device_status = get_device(pathname, device);
    if (get_device_status == E_DEVICE_PATH) {
        return E_DEVICE_PATH;
    }
    // Check for the first open Stream in pcb->streams
    for (int i=0; i<sizeof(currentPCB->streams)/sizeof(currentPCB->streams[0]); i++) {
        // Stream is available
        if ((*currentPCB->streams)[i].status == 0)
        {
            // Pass the file descriptor, set the stream->device, and call stream->device->fopen
            *fd = i;
            (*currentPCB->streams)[i].device = device;
            (*currentPCB->streams)[i].pathname = pathname;
        }
    }
    // Then call the device->fopen function
    int stream_status;
    if (stream_status != E_SUCCESS)
    {
        return E_INIT_STREAM;
    }

}