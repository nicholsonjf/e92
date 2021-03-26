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

int fopen(char *pathname, file_descriptor *fd)
{
    // Parse the pathname and create a struct Stream that holds the right device, etc
    int stream_status;;
    if (stream_status != E_SUCCESS)
    {
        return E_INIT_STREAM;
    }

}