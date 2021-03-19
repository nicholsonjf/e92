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

// Check for long filenames and skip them.
int dir_ls(void) {
    uint32_t fsc = first_sector_of_cluster(cwd);
    uint8_t first_block[512];
    struct sdhc_card_status *card_status = myMalloc(sizeof(struct sdhc_card_status));
    sdhc_read_single_block(rca, fsc, card_status, first_block);
    struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)first_block;
    uint8_t *pfname;
    while (1) {
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
            return E_SUCCESS;
        }
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
            dir_entry++;
            continue;
        }
        uint8_t dir_attr_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
        if ( dir_attr_masked == DIR_ENTRY_ATTR_LONG_NAME) {
            myprintf("%s\n", "*Skipped long filename*");
            dir_entry++;
            continue;
        }
        int ffn_result = friendly_file_name(dir_entry, &pfname);
        myprintf("%s\n", pfname);
        dir_entry++;
    }
    myFree(pfname);
    myFree(card_status);
    return E_SUCCESS;
}

/**
 * Takes an 8.3 filename and copies the user friendly version into a user provided pointer address.
 * Allowed characters are [A-Z,0-9]
 * Param dir_entry: pointer to a valid FAT32 directory entry
 * Param friendly_name: pointer to a pointer allocated at least sizeof(uint8_t)*13
 * Returns: E_SUCCESS if all went well
 * Error: If an illegal character is encountered at the beginning of the filename the function
 * returns E_FILE_NAME_INVALID
 */ 
int friendly_file_name(struct dir_entry_8_3 *dir_entry, uint8_t **friendly_name) {
    // Enough memory for the short filename (11) + the dot (1) + NULL terminator (1)
    uint8_t *p = myMalloc(sizeof(uint8_t)*13);
    myMemset(p, 0x0, sizeof(p));
    int endfn;
    if (dir_entry->DIR_Name[0] < 0x41 || dir_entry->DIR_Name[0] > 0x5A) {
        return E_FILE_NAME_INVALID;
    }
    // Copy filename into p
    for (int i=0; i<7; i++) {
        if (dir_entry->DIR_Name[i] >= 0x41 && dir_entry->DIR_Name[i] <= 0x5A) {
            p[i] = dir_entry->DIR_Name[i];
        }
        else {
            // End of the filename
            // Index of the first non uppercase character, aka where the dot should go
            endfn = i;
            break;
        }
    }
    // Check if there's a file extension
    if (dir_entry->DIR_Name[8] >= 0x41 && dir_entry->DIR_Name[8] <= 0x5A) {
        // Add the dot separator to p
        p[endfn] = '.';
        for (int i=8; i<11; i++) {
            // Add each extension letter to p
            if (dir_entry->DIR_Name[i] >= 0x41 && dir_entry->DIR_Name[i] <= 0x5A) {
                p[++endfn] = dir_entry->DIR_Name[i];
            }
        }
    }
    *friendly_name = p; 
    return E_SUCCESS;
}