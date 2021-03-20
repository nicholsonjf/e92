/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "shell.h"
#include "myFAT32driver.h"


int initDevIO (void) {
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS) {
        return initFAT_status;
    }
    return E_SUCCESS;
}