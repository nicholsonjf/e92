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
    // Pointer to hold the pretty filename
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    while (1) {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    return E_SUCCESS;
                }
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                int ffn_result = entry_to_filename(dir_entry, filename_wrapper);
                myprintf("%s\n", filename_wrapper->combined);
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t cwdFATentry = read_FAT_entry(rca, cwd);
        // Set cwd to the current directory's FAT entry and continue iteration
        cwd = cwdFATentry;
    }
    myFree(filename_wrapper);
    return E_SUCCESS;
}

int chr_8_3_valid(uint8_t c) {
    if ((c < 0x20) ||
    (c == 0x22) ||
    (c >= 0x2A && c <= 0x2F) ||
    (c >= 0x3A && c <= 0x3F) ||
    (c >= 0x5B && c <= 0x5D) ||
    (c >= 0x61 && c <= 0x7A) ||
    (c == 0x7C)) {
        return E_CHR_INVALID_8_3;
    }
    return E_SUCCESS;
}


int entry_to_filename(struct dir_entry_8_3 *dir_entry, Filename_8_3_Wrapper *filename_wrapper) {
    memset(filename_wrapper, 0x0, sizeof(Filename_8_3_Wrapper));
    int dot_index;
    int ext_index = 8;
    // Check if first character in filename is a space ' ' or period '.'
    if (dir_entry->DIR_Name[0] == 0x20 || dir_entry->DIR_Name[0] == 0x2E) {
        return E_FILE_NAME_INVALID;
    }
    // Copy filename into filename_wrapper.name and filename_wrapper.combined
    for (int i=0; i<ext_index; i++) {
        if (chr_8_3_valid(dir_entry->DIR_Name[i]) == E_SUCCESS) {
            filename_wrapper->combined[i] = dir_entry->DIR_Name[i];
            filename_wrapper->name[i] = dir_entry->DIR_Name[i];
        }
        else {
            dot_index = i;
        }
    }
    // Check if there's a file extension and copy it into filename_wrapper.name and filename_wrapper.combined
    if (chr_8_3_valid(dir_entry->DIR_Name[ext_index]) == E_SUCCESS) {
        // Put a dot into filename_wrapper->combined
        filename_wrapper->combined[dot_index] = '.';
        // 
        for (int i=ext_index, j=0, k=dot_index+1; i<ext_index+3; i++, j++, k++) {
            // Add each extension letter
            if (chr_8_3_valid(dir_entry->DIR_Name[i]) == E_SUCCESS) {
                filename_wrapper->ext[j] = dir_entry->DIR_Name[i];
                filename_wrapper->combined[k] = dir_entry->DIR_Name[i];
            }
        }
    }
    return E_SUCCESS;
}

// filename is assumed to be null terminated
int create_filename_wrapper(char *filename, Filename_8_3_Wrapper *filename_wrapper) {
    // Check if first character in filename is a space ' ' or period '.'
    if (filename[0] == 0x20 || filename[0] == 0x2E) {
        return E_FILE_NAME_INVALID;
    }
    int is_ext = 0;
    for (int i=0; i<sizeof(filename)-1; i++) {
        if (chr_8_3_valid(filename[i]) != E_SUCCESS) {
            return E_FILE_NAME_INVALID;
        }
        // If character is a period
        if (filename[i] == 0x2E) {
            is_ext = 1;
            filename_wrapper->combined[i] = 0x2E;
            continue;
        }
        if (is_ext == 0) {
            filename_wrapper->combined[i] = filename[i];
            filename_wrapper->name[i] = filename[i];
        }
        if (is_ext == 1) {
            filename_wrapper->combined[i] = filename[i];
            filename_wrapper->ext[i] = filename[i];
        }
    }
    return E_SUCCESS;
}

// Assumes filename_wrapper has no invalid characters, because it was created by create_filename_wrapper
int filename_to_entry(Filename_8_3_Wrapper *filename_wrapper, struct dir_entry_8_3 *dir_entry) {
    // First zero out the entry's DIR_Name
    memset(dir_entry, 0x0, 11);
    // Copy fname_wrapper.name into dir_entry->DIR_Name
    int ext_index = 8;
    for (int i=0; i<ext_index; i++) {
        dir_entry->DIR_Name[i] = filename_wrapper->name[i];
    }
    // Copy fname_wrapper.ext into dir_entry->DIR_Name
    // If there is no extension, zeroes will be copied over which is fine
    for (int i=ext_index; i<ext_index+3; i++) {
        dir_entry->DIR_Name[i] = filename_wrapper->name[i];
    }
    return E_SUCCESS;
}

// Check for long filenames and skip them.
int dir_find_file(char *filename, uint32_t *firstCluster) {
    // Pointer to hold the pretty filename
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    while (1) {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                // End of dir, return error
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    return E_FILE_NOT_IN_CWD;
                }
                // Entry not used, continue
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Long dir entry, not supported, return error
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Entry is a dir, return 
                uint8_t dir_attr_dir_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_DIRECTORY;
                if ( dir_attr_dir_masked == DIR_ENTRY_ATTR_DIRECTORY) {
                    return E_FILE_IS_DIRECTORY;
                }
                // This is an in-use 8.3 file, check to see if it matches
                int ffn_result = entry_to_filename(dir_entry, filename_wrapper); // Filename + extension of entry at current point in iteration
                int fnamecmp  = strncmp((const char*)filename_wrapper->combined, (const char*)filename, (size_t)sizeof(filename));
                if (fnamecmp == 0) {
                    *firstCluster = (uint32_t)dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
                    return E_SUCCESS;
                }
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t cwdFATentry = read_FAT_entry(rca, cwd);
        // Set cwd to the current directory's FAT entry and continue iteration
        cwd = cwdFATentry;
    }
    myFree(filename_wrapper);
    return E_SUCCESS;
}

int dir_create_file(char *filename) {
    // Return an error if requested filename uses an invalid character
    for (int i=0; i<sizeof(filename-1); i++) {
        if (chr_8_3_valid(filename[i]) != E_SUCCESS) {
            return E_FILE_NAME_INVALID;
        }
    }
    uint32_t *fcluster = myMalloc(sizeof(uint32_t));
    // Check if the filename already exists
    int ffr = dir_find_file(filename, fcluster);
    if (ffr == E_SUCCESS || ffr == E_FILE_IS_DIRECTORY) {
        return E_FILE_EXISTS;
    }
    // Malloc space for the filename wrapper
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    int filename_wrapper_sts = create_filename_wrapper(filename, filename_wrapper);
    if (filename_wrapper_sts != E_SUCCESS) {
        return E_FILE_NAME_INVALID;
    }
    // Find first free entry
    while (1) {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index <= sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index <= dir_entries_per_sector) {
                // Unused entry found, create file
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    // Clear the entry attribute byte
                    dir_entry->DIR_Attr = 0x0;
                    // Set the file size to zero
                    dir_entry->DIR_FileSize = 0x0;
                    // Set the filename
                    dir_entry->DIR_Name[0] = *filename_wrapper->name;
                    dir_entry->DIR_Name[8] = *filename_wrapper->ext;
                }
                // Last entry in directory found.
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    // But not the last entry in the sector. Set the name and tag the next entry DIR_ENTRY_LAST_AND_UNUSED
                    if (entry_index != dir_entries_per_sector) {
                        //TODO
                    }
                    // Last entry in directory, and the sector. Set the name and tag the first entry in the
                    // next sector DIR_ENTRY_LAST_AND_UNUSED
                    //TODO
                }
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t cwdFATentry = read_FAT_entry(rca, cwd);
        // Set cwd to the current directory's FAT entry and continue iteration
        cwd = cwdFATentry;
    }
    myFree(filename_wrapper);
    myFree(fcluster);
    return E_SUCCESS;
}