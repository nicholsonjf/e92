/**
 * mySDHCdriver.c
 * A driver for a micro SDHC card
 * 
 * Author: James Nicholson
 */

#include "mySDHCdriver.h"
#include "microSD.h"
#include "bootSector.h"
#include "utils.h"
#include "my-malloc.h"
#include "FAT.h"
#include <stdint.h>

/**
 * Indicates whether the file system is mounted: 0 if false, 1 if true.
 */
int file_structure_mounted = 0;

/**
 * Relative Card Address of currently mounted card.
 */
uint32_t rca;

int file_structure_mount(void)
{
    // my_card_status is freed in file_structure_umount
    struct sdhc_card_status *my_card_status = myMalloc(sizeof(struct sdhc_card_status));
    if (file_structure_mounted)
    {
        return E_FILE_STRUCT_MOUNTED;
    }
    microSDCardDetectConfig();
    int microSDdetected = microSDCardDetectedUsingSwitch();
    if (!microSDdetected)
    {
        return E_NO_MICRO_SD;
    }
    microSDCardDisableCardDetectARMPullDownResistor();
    rca = sdhc_initialize();
    file_structure_mounted = 1;
    return E_SUCCESS;
}

int file_structure_umount(void)
{
    if (!file_structure_mounted)
    {
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    invalidate_entire_FAT_cache();
    sdhc_command_send_set_clr_card_detect_connect(rca);
    myFree(my_card_status);
    file_structure_mounted = 0;
    return E_SUCCESS;
}

int initSDHC(void)
{
    file_structure_mount();
    return E_SUCCESS;
}