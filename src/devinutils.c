/**
 * devutils.c
 * Device independent utilities
 * 
 * Author: James Nicholson
 */

#include "devinutils.h"
#include "pcb.h"
#include "utils.h"
#include <string.h>

int get_device(char *pathname, Device *device)
{
    for (int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        if (strcmp(pathname, devices[i].pathname) == 0)
        {
            device = devices[i].device;
        }
    }
    return E_DEVICE_PATH;
}

// Get the first available stream in pcb->streams or return an error
int get_available_stream(file_descriptor *fd)
{
    // Check for the first open Stream in pcb->streams
    for (int i = 0; i < sizeof(currentPCB->streams) / sizeof(currentPCB->streams[0]); i++)
    {
        // Stream is available
        if ((*currentPCB->streams)[i].status == 0)
        {
            // Pass the file descriptor back to the caller
            *fd = i;
            return E_SUCCESS;
        }
    }
    return E_MAX_STREAMS;
}