/**
 * myFAT32driver.c
 * A driver for the FAT32 file system
 * 
 * Author: James Nicholson
 */

#include "microSD.h"
#include "SDHC_FAT32_Files.h"
#include "shell.h"
#include "myFAT32driver.h"
#include "my-malloc.h"

/**
 * Initialize a structure to hold additional SDHC status data, defined in initFAT()
 */
struct sdhc_card_status *card_status;

int initFAT(void) {
    card_status = myMalloc(sizeof(struct sdhc_card_status));
    int microSDmounted = file_structure_mount();
    if (microSDmounted != E_SUCCESS) {
        return microSDmounted;
    }
    int set_cwd = dir_set_cwd_to_root();
    if (set_cwd != E_SUCCESS) {
        return set_cwd;
    }
    return E_SUCCESS;
}