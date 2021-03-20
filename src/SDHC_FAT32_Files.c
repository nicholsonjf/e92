#include <string.h>
#include "SDHC_FAT32_Files.h"
#include "shell.h"
#include "microSD.h"
#include "bootSector.h"
#include "breakpoint.h"
#include "directory.h"
#include "my-malloc.h"
#include "FAT.h"
#include "myFAT32driver.h"

/////// GLOBALS

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

/**
 * Number of sectors in a cluster.
 */
uint8_t dir_entries_per_sector;

/////// FUNCTIONS

int file_structure_mount(void) {
    if (file_structure_mounted) {
        return E_FILE_STRUCT_MOUNTED;
    }
    microSDCardDetectConfig();
    int microSDdetected = microSDCardDetectedUsingSwitch();
    if (!microSDdetected) {
        return E_NO_MICRO_SD;
    }
    microSDCardDisableCardDetectARMPullDownResistor();
    rca = sdhc_initialize();
    file_structure_mounted = 1;
    dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    return E_SUCCESS;
}

int file_structure_umount(void) {
    if (!file_structure_mounted) {
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    // TODO call flush_cache
    sdhc_command_send_set_clr_card_detect_connect(rca);
    myFree(card_status);
    file_structure_mounted = 0;
    return E_SUCCESS;
}

int dir_set_cwd_to_root(void) {
    if (!file_structure_mounted) {
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    cwd = root_directory_cluster;
    return E_SUCCESS;
}

// Check for long filenames and skip them.
int dir_ls(void) {
    // TODO redefine scope of allowable characters ~ is alowed
    // Nothing less than 20
    // Pointer to hold the pretty filename
    uint8_t *pfname;
    while (1) {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, card_status, sector_data);
            // TODO check read_status and go to __BKPT if failure
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    return E_SUCCESS;
                }
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    dir_entry++;
                    continue;
                }
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
                    dir_entry++;
                    continue;
                }
                int ffn_result = friendly_file_name(dir_entry, &pfname);
                myprintf("%s\n", pfname);
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t cwdFATentry = read_FAT_entry(rca, cwd);
        // Set cwd to the current directory's FAT entry and continue iteration
        cwd = cwdFATentry;
    }
    myFree(pfname);
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
    // Enough to hold the short filename (11) + the dot (1) + NULL terminator (1)
    uint8_t p[13];
    myMemset(&p[0], 0x0, sizeof(p));
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
    *friendly_name = &p[0]; 
    return E_SUCCESS;
}

int dir_find_file(char *filename, uint32_t *firstCluster) {
    //TODO convert user provided filename to UPPERCASE before searching
    uint32_t fsc = first_sector_of_cluster(cwd);
    uint8_t first_block[512];
    sdhc_read_single_block(rca, fsc, card_status, first_block);
    struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)first_block;
    uint8_t *pfname;
    while (1) {
        // End of dir, return error
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
            return E_FILE_NOT_IN_CWD;
        }
        // Entry not used, continue
        if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
            dir_entry++;
            continue;
        }
        // Long dir entry, not supported, return error
        uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
        if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
            dir_entry++;
            continue;
        }
        // Entry is a dir, return 
        uint8_t dir_attr_dir_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_DIRECTORY;
        if ( dir_attr_dir_masked == DIR_ENTRY_ATTR_DIRECTORY) {
            return E_FILE_IS_DIRECTORY;
        }
        // This is an in-use 8.3 file, check to see if it matches
        int ffn_result = friendly_file_name(dir_entry, &pfname); // Filename + extension of entry at current point in iteration
        int fnamecmp  = strncmp((const char*)pfname, (const char*)filename, (size_t)sizeof(filename));
        if (fnamecmp == 0) {
            *firstCluster = (uint32_t)dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
            return E_SUCCESS;
        }
        dir_entry++;
    }
    myFree(pfname);
}

int dir_create_file(char *filename) {
    uint32_t *fcluster = myMalloc(sizeof(uint32_t));
    // Check if it exists (check sector, expand if necessarry)
    if (dir_find_file(filename, fcluster) == 0) {
        return E_FILE_EXISTS;
    }
    // Then do another pass to find first free entry
    myFree(fcluster);
}