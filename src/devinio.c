/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "devinio.h"
#include "my-malloc.h"
#include "utils.h"
#include "pcb.h"
#include "devinutils.h"
#include <string.h>


int myfopen(char *pathname, file_descriptor *fd)
{
    Device *device;
    int get_device_status = get_device(pathname, &device);
    if (get_device_status == E_DEVICE_PATH) {
        return E_DEVICE_PATH;
    }
    int fopen_status = device->fopen(pathname, fd);
    if (fopen_status != E_SUCCESS) {
        return fopen_status;
    }
    // stream was successfully defined and is located at index *fd
    // finish defining the stream members
    (currentPCB->streams)[*fd].device = device;
    (currentPCB->streams)[*fd].pathname = pathname;
    // Stream is in use
    (currentPCB->streams)[*fd].in_use = 1;
    return E_SUCCESS;
}

int myfdelete(char *pathname)
{
    Device *device;
    int get_device_status = get_device(pathname, &device);
    if (get_device_status == E_DEVICE_PATH)
    {
        return get_device_status;
    }
    int fdelete_status = device->fdelete(pathname);
    if (fdelete_status != E_SUCCESS)
    {
        return fdelete_status;
    }
    return E_SUCCESS;
}

int myfcreate(char *pathname)
{
    Device *device;
    int get_device_status = get_device(pathname, &device);
    if (get_device_status == E_DEVICE_PATH)
    {
        return get_device_status;
    }
    int fcreate_status = device->fcreate(pathname);
    if (fcreate_status != E_SUCCESS)
    {
        return fcreate_status;
    }
    return E_SUCCESS;
}

int myfclose(file_descriptor *fd)
{
    Device *device = (currentPCB->streams)[*fd].device;
    int fclose_status = device->fclose(fd);
    if (fclose_status != E_SUCCESS)
    {
        return fclose_status;
    }
    (currentPCB->streams)[*fd].device = (Device *)0;
    (currentPCB->streams)[*fd].pathname = (void *)0;
    (currentPCB->streams)[*fd].in_use = 0;
    return E_SUCCESS;
}