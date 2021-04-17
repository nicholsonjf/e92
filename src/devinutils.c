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
#include "myLEDdriver.h"
#include "myFAT32driver.h"
#include "myPBdriver.h"
#include <string.h>

int get_device(char *pathname, Device **device)
{
    char sw1_path[] = "/dev/sw1";
    char sw2_path[] = "/dev/sw2";
    char ledy_path[] = "/dev/ledy";
    char ledg_path[] = "/dev/ledg";
    char ledb_path[] = "/dev/ledb";
    char ledo_path[] = "/dev/ledo";
    char uart_path[] = "/dev/uart";
    // Check if the pathname matches one of the pre-initialized devices
    if (strcmp(pathname, &sw1_path[0]) == 0)
    {
        *device = &SW1;
        return E_SUCCESS;
    }
    if (strcmp(pathname, &sw2_path[0]) == 0)
    {
        *device = &SW2;
        return E_SUCCESS;
    }
    if (strcmp(pathname, &ledy_path[0]) == 0)
    {
        *device = &LEDYellow;
        return E_SUCCESS;
    }
    if (strcmp(pathname, &ledg_path[0]) == 0)
    {
        *device = &LEDGreen;
        return E_SUCCESS;
    }
    if (strcmp(pathname, &ledb_path[0]) == 0)
    {
        *device = &LEDBlue;
        return E_SUCCESS;
    }
    if (strcmp(pathname, &ledo_path[0]) == 0)
    {
        *device = &LEDOrange;
        return E_SUCCESS;
    }
    if (strncmp(pathname, &uart_path[0], strlen(const &uart_path[0]) + 1) == 0)
    {
        *device = &UART;
        return E_SUCCESS;
    }
    // If the first char in pathname is "/" and it's not an exact match with an already
    // initialized device it is assumed to be referencing a FAT32 file 
    if (*pathname == '/')
    {
        *device = &FAT32;
        return E_SUCCESS;
    }
    // If no match return an error
    return E_DEVICE_PATH;
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
