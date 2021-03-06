/**
 * devinit.c
 * Device initialization
 * 
 * Author: James Nicholson
 */

#include "devinit.h"
#include "devinutils.h"
#include "SDHC_FAT32_Files.h"
#include "pcb.h"
#include "my-malloc.h"
#include "myFAT32driver.h"
#include "myLEDdriver.h"
#include "myPBdriver.h"
#include "mySDHCdriver.h"
#include "utils.h"
#include <string.h>


int initDevices(void)
{
    /**
     * Init the 32GB microSDHC card
     */
    int initSDHC_status = initSDHC();
    if (initSDHC_status != E_SUCCESS)
    {
        return initSDHC_status;
    }
    /**
     * Init the K70 LED lights
     */
    int initLED_status = initLED();
    if (initLED_status != E_SUCCESS)
    {
        return initLED_status;
    }
    /**
     * Init the K70 push buttons
     */
    int initPB_status = initPB();
    if (initPB_status != E_SUCCESS)
    {
        return initPB_status;
    }
    /**
     * Init the FAT32 file system
     */
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS)
    {
        return initFAT_status;
    }
    return E_SUCCESS;
}

int exitDevices(void) {
    if (file_structure_mounted)
    {
        int unmount = file_structure_umount();
        if (unmount != E_SUCCESS)
        {
            return unmount;
        }
    }
    return E_SUCCESS;
}
