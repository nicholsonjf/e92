/**
 * devutils.c
 * Device independent utilities
 * 
 * Author: James Nicholson
 */

#include "devinutils.h"
#include "devinit.h"
#include "pcb.h"
#include "utils.h"
#include "myFAT32driver.h"
#include <string.h>

int get_device(char *pathname, Device **device)
{
    for (int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        // Check if the pathname matches one of the devices in the devices array
        if (strcmp(pathname, devices[i].pathname) == 0)
        {
            *device = devices[i].device;
            return E_SUCCESS;
        }

    }
    // if not, assume the file is FAT32 and use that driver
    *device = &FAT32;
    return E_SUCCESS;
}

// Get the first available stream in pcb->streams or return an error
int get_available_stream(file_descriptor *fd)
{
    // Check for the first open Stream in pcb->streams
    for (int i = 0; i < sizeof(currentPCB->streams) / sizeof(currentPCB->streams[0]); i++)
    {
        // Stream is available
        if ((currentPCB->streams)[i].in_use == 0)
        {
            // Pass the file descriptor back to the caller
            *fd = i;
            return E_SUCCESS;
        }
    }
    return E_MAX_STREAMS;
}