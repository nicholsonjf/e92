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
    FAT32->fgetc = fgetc;
    FAT32->fputc = fputc;
    FAT32->fclose = fclose;
    return E_SUCCESS;
}

int fgetc(void)
{
    return E_SUCCESS;
}

int fputc(void)
{
    return E_SUCCESS;
}

int fclose(void)
{
    return E_SUCCESS;
}