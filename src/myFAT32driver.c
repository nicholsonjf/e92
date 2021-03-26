/**
 * myFAT32driver.c
 * A driver for the FAT32 file system
 * 
 * Author: James Nicholson
 */

#include "myFAT32driver.h"
#include "microSD.h"
#include "SDHC_FAT32_Files.h"
#include "my-malloc.h"
#include "utils.h"

/**
 * Initialize a structure to hold additional SDHC status data, defined in initFAT()
 */
struct sdhc_card_status *card_status;

int fatfgetc(void)
{
    return E_SUCCESS;
}

int fatfputc(void)
{
    return E_SUCCESS;
}

int fatfclose(void)
{
    return E_SUCCESS;
}

int initFAT(void) {
    // Mount the SDHC card
    card_status = myMalloc(sizeof(struct sdhc_card_status));
    int microSDmounted = file_structure_mount();
    if (microSDmounted != E_SUCCESS) {
        return microSDmounted;
    }
    // Set cwd to the root cluster
    int set_cwd = dir_set_cwd_to_root();
    if (set_cwd != E_SUCCESS) {
        return set_cwd;
    }
    // Define the struct Device FAT32
    FAT32->fgetc = fatfgetc;
    FAT32->fputc = fatfputc;
    FAT32->fclose = fatfclose;
    return E_SUCCESS;
}