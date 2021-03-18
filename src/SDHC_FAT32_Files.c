#include "SDHC_FAT32_Files.h"
#include "shell.h"
#include "microSD.h"
#include "bootSector.h"
#include "breakpoint.h"
#include "directory.h"
#include "my-malloc.h"


/////// GLOBALS

/**
 * Indicates whether the file system is mounted: 0 if true, 1 if false.
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


/////// FUNCTIONS

int file_structure_mount(void) {
    if (file_structure_mounted) {
        __BKPT();
        return E_FILE_STRUCT_MOUNTED;
    }
    microSDCardDetectConfig();
    int microSDdetected = microSDCardDetectedUsingSwitch();
    if (!microSDdetected) {
        __BKPT();
        return E_NO_MICRO_SD;
    }
    microSDCardDisableCardDetectARMPullDownResistor();
    rca = sdhc_initialize();
    file_structure_mounted = 1;
    return E_SUCCESS;
}

int file_structure_umount(void) {
    if (!file_structure_mounted) {
        __BKPT();
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    sdhc_command_send_set_clr_card_detect_connect(rca);
    file_structure_mounted = 0;
    return E_SUCCESS;
}

int dir_set_cwd_to_root(void) {
    cwd = root_directory_cluster;
    return E_SUCCESS;
}

int dir_ls(void) {
    uint32_t fsc = first_sector_of_cluster(cwd);
    uint8_t first_block[512];
    struct sdhc_card_status *card_status = myMalloc(sizeof(struct sdhc_card_status));
    sdhc_read_single_block(rca, fsc, card_status, first_block);
    struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)first_block;
    while (1) {
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
            return E_SUCCESS;
        }
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
            dir_entry++;
            continue;
        }
        myprintf("%s\n", dir_entry->DIR_Name);
        __BKPT();
        dir_entry++;

    }
    myFree(card_status);
    return E_SUCCESS;

}