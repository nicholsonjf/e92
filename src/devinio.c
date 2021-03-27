/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "devinio.h"
#include "my-malloc.h"
#include "utils.h"
#include <string.h>


int myfopen(char *pathname, file_descriptor *fd)
{
    Device *device = myMalloc(sizeof(Device));
    int get_device_status = get_device(pathname, device);
    if (get_device_status == E_DEVICE_PATH) {
        return E_DEVICE_PATH;
    }
    int fopen_status = device->fopen(pathname, fd);
    if (fopen_status != E_SUCCESS) {
        return fopen_status;
    }
    currentPCB->streams[*fd].device = device;
    currentPCB->streams[*fd].pathname = pathname;
    // Stream is in use
    currentPCB->streams[i].status = 1;
    return E_SUCCESS;
}

int myfdelete(char *pathname)
{
    Device *device = myMalloc(sizeof(Device));
    int get_device_status = get_device(pathname, device);
    if (get_device_status == E_DEVICE_PATH)
    {
        return E_DEVICE_PATH;
    }
    int fopen_status = device->fopen(pathname, fd);
    if (fopen_status != E_SUCCESS)
    {
        return fopen_status;
    }
    currentPCB->streams[*fd].device = device;
    currentPCB->streams[*fd].pathname = pathname;
    // Stream is in use
    currentPCB->streams[i].status = 1;
    return E_SUCCESS;
}