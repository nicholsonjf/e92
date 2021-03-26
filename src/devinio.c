/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "shell.h"
#include "myFAT32driver.h"
#include "devinio.h"


int initDevIO (void) {
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS) {
        return initFAT_status;
    }
    return E_SUCCESS;
}

struct deviceDict {
    char *pathname;
    Device *device;
} devices[] = {
    {"/", FAT32},
    {"/dev/pushbutton/sw1", SW1},
    {"/dev/pushbutton/sw2", SW2},
    {"/dev/led/yellow", LEDYellow},
    {"/dev/led/orange", LEDOrange},
    {"/dev/led/blue", LEDBlue},
    {"/dev/led/green", LEDGreen}
    };

Device get_device(char *pathname)
{
    for (int i=0; i<sizeof(devices) / sizeof(devices[0]); i++)
    {
        if (strcmp(pathname, devices[i].pathname) == 0)
        {
            return devices[i].device;
        }
    }
    return NULL;
}

int fopen(char *pathname, file_descriptor *fd)
{
    Device device = get_device(pathname);
    if (device == NULL) {
        return E_DEVICE_PATH
    }
    // Check for the first open Stream in pcb->streams
    for (int i=0; i<sizeof(pcb->streams)/sizeof(pcb->streams[0]); i++) {
        // Stream is available
        if (pcb->streams[i]->status = 0) {
            // Pass the file descriptor, set the stream->device, and call stream->device->fopen
            *fd = i;
            pcb->streams[i]->device = device;
            pcb->streams[i]->pathname = pathname;
        }
    }
    // Then call the device->fopen function
    int stream_status;
    if (stream_status != E_SUCCESS)
    {
        return E_INIT_STREAM;
    }

}