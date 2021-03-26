/**
 * devinio.h
 * Device independent IO
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

int initDevIO(void)
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

int myfopen(char *pathname, file_descriptor *fd)
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
            break;
        }
    }
    // Then call the device->fopen function
    int stream_status;
    if (stream_status != E_SUCCESS)
    {
        return E_INIT_STREAM;
    }
    return E_SUCCESS;
}

int myfdelete(char *pathname)
{
    if (pathname[0] != '/') {
        return E_FILE_NAME_INVALID;
    }
    const char *filename = pathname[1];
    size_t pathname_len = strlen(filename);
    if (pathname_len < 1 || pathname_len > 11) {
        return E_FILE_NAME_TOO_LONG;
    }
    int delete_file = dir_delete_file(filename);
    if (delete_file != E_SUCCESS)
    {
        return delete_file;
    }
    return E_SUCCESS;
}