/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "microSD.h"
#include "SDHC_FAT32_Files.h"
#include "shell.h"

/**
 * Indicates whether the file system is mounted: 0 if false, 1 if true.
 */
int file_structure_mounted;

/**
 * Relative Card Address of currently mounted card.
 */
uint32_t rca;

/**
 * Cluster number of the current working directory.
 */
uint32_t cwd;


int initFAT(void) {
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

int initDevIO (void) {
    int initFAT_status = initFAT();
    if (initFAT_status != E_SUCCESS) {
        return initFAT_status;
    }
    return E_SUCCESS;
}