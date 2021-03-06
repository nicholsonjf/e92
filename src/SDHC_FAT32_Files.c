#include "SDHC_FAT32_Files.h"
#include "utils.h"
#include "microSD.h"
#include "bootSector.h"
#include "breakpoint.h"
#include "directory.h"
#include "my-malloc.h"
#include "FAT.h"
#include "myFAT32driver.h"
#include "fsinfo.h"
#include "devinutils.h"
#include "pcb.h"
#include "mySDHCdriver.h"
#include <string.h>


/**
 * Cluster number of the current working directory.
 */
uint32_t cwd;

int dir_set_cwd_to_root(void) {
    if (!file_structure_mounted) {
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    cwd = root_directory_cluster;
    return E_SUCCESS;
}

// Check for long filenames and skip them.
int dir_ls(void) {
    struct sdhc_card_status my_card_status;
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    uint32_t current_cluster_number = cwd;
    // Pointer to hold the pretty filename.
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    // TODO don't loop over every cluster, traverse the linked list of directory clusters using the FAT
    while (current_cluster_number <= total_data_clusters + 1)
    {
        uint32_t sector_num = first_sector_of_cluster(current_cluster_number);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, &my_card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                // Last unused, return success
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    return E_SUCCESS;
                }
                // Unused entry, continue to next entry
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // A directory, continue
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // A valid file, output filename to console
                int etf_result = entry_to_filename(dir_entry, filename_wrapper);
                if (etf_result != E_SUCCESS) {
                    return etf_result;
                }
                myprintf("%s\n", filename_wrapper->combined);
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t current_cluster_FAT_entry = read_FAT_entry(rca, cwd);
        if (current_cluster_FAT_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE)
        {
            return E_FILE_NOT_IN_CWD;
        }
        if (current_cluster_FAT_entry == FAT_ENTRY_DEFECTIVE_CLUSTER)
        {
            // Fatal error
            __BKPT();
        }
        // FAT entry is in use and points to next cluster
        // Set current_cluster_number to the current directory's FAT entry and continue iteration
        current_cluster_number = current_cluster_FAT_entry;
    }
    myFree(filename_wrapper);
    return E_SUCCESS;
}

int chr_8_3_valid(uint8_t c) {
    // Exclude the NULL terminator, we'll handle that in create_filename_wrapper
    if ((c < 0x20 && c != 0) ||
        (c == 0x22) ||
        // Exclude the period from this validator, we will handle it in create_filename_wrapper
        (c >= 0x2A && c <= 0x2F && c != 0x2E) ||
        (c >= 0x3A && c <= 0x3F) ||
        (c >= 0x5B && c <= 0x5D) ||
        (c >= 0x61 && c <= 0x7A) ||
        (c == 0x7C))
    {
        return E_CHR_INVALID_8_3;
    }
    return E_SUCCESS;
}


int entry_to_filename(struct dir_entry_8_3 *dir_entry, Filename_8_3_Wrapper *filename_wrapper) {
    memset(filename_wrapper, 0x0, sizeof(Filename_8_3_Wrapper));
    int filename_end_index;
    int ext_index = 8;
    // Check if first character in filename is a space ' ' or period '.'
    if (dir_entry->DIR_Name[0] == 0x20 || dir_entry->DIR_Name[0] == 0x2E) {
        return E_FILE_NAME_INVALID;
    }
    // Copy filename into filename_wrapper.name and filename_wrapper.combined
    for (int i=0; i<ext_index; i++) {
        // The chr is "valid" and not a period or zero
        if (chr_8_3_valid(dir_entry->DIR_Name[i]) == E_SUCCESS && dir_entry->DIR_Name[i] != 0x0 && dir_entry->DIR_Name[i] != 0x2E)
        {
            filename_wrapper->combined[i] = dir_entry->DIR_Name[i];
            filename_wrapper->name[i] = dir_entry->DIR_Name[i];
        }
        else {
            filename_end_index = i;
            break;
        }
    }
    // Check if there's a valid file extension and copy it into filename_wrapper.name and filename_wrapper.combined
    int ext_first_chr = dir_entry->DIR_Name[ext_index];
    // The first chr of the ext is "valid" and not a period, zero, or space
    if (chr_8_3_valid(ext_first_chr) == E_SUCCESS && ext_first_chr != 0x0 && ext_first_chr != 0x2E && ext_first_chr != 0x20) {
        // Put a dot into filename_wrapper->combined
        filename_wrapper->combined[filename_end_index] = '.';
        // Copy in the extension characters
        for (int i=ext_index, j=0, k=filename_end_index+1; i<ext_index+3; i++, j++, k++) {
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
    // Caller is responsible for freeing filename_wrapper!
    memset(filename_wrapper, 0x0, sizeof(Filename_8_3_Wrapper));
    // Check if first character in filename is a space ' ' or period '.'
    if (filename[0] == 0x20 || filename[0] == 0x2E || filename[0] == 0x0) {
        return E_FILE_NAME_INVALID;
    }
    // Initialize to zero. Because first chr cannot be a period, if period_index > 0 it means the
    // filename may have a valid extension and the next chr will need to be checked
    int period_index = 0;
    int max_file_name_length = 8;
    int max_file_ext_length = 3;
    int max_combined_length = max_file_name_length + max_file_ext_length;
    for (int i=0; i<max_combined_length; i++) {
        // Null terminator reached
        if (i > 0 && filename[i] == 0x0) {
            return E_SUCCESS;
        }
        // First period in the filename reached, and it is not the first letter in the filename
        if (filename[i] == 0x2E && period_index == 0) {
            // If the following chr is valid it's safe to continue, else return here
            if (chr_8_3_valid(filename[i+1]) != E_SUCCESS) {
                // This is a valid filename with no extension
                return E_SUCCESS;
            }
            period_index = i;
            // Add the period to filename_wrapper->combined and continue to write ext
            filename_wrapper->combined[i] = 0x2E;
            continue;
        }
        // Second period in the filename reached, which is not supported, return filename invalid
        if (filename[i] == 0x2E && period_index > 0)
        {
            return E_FILE_NAME_INVALID;
        }
        if (chr_8_3_valid(filename[i]) != E_SUCCESS)
        {
            return E_FILE_NAME_INVALID;
        }
        // Haven't hit the period yet, continue copying chrs to ->name
        if (period_index == 0) {
            filename_wrapper->combined[i] = filename[i];
            filename_wrapper->name[i] = filename[i];
        }
        // Reached an extension, start copying chrs to ->ext
        if (period_index >= 1 && i-period_index <= max_file_ext_length) {
            filename_wrapper->combined[i] = filename[i];
            // Start index of ->ext is 0, a.k.a current position minus one (the period) minus period_index (file_name)
            filename_wrapper->ext[i-1-period_index] = filename[i];
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
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    struct sdhc_card_status my_card_status;
    uint32_t current_cluster_number = cwd; 
    // Pointer to hold the pretty filename
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    while (current_cluster_number <= total_data_clusters + 1)
    {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, &my_card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                // Last entry in the directory and it is unused. Return file not found.
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    myFree(filename_wrapper);
                    return E_FILE_NOT_IN_CWD;
                }
                // Entry not used, continue
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Long dir entry, not supported, continue
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if ( dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Entry is a dir (nested directories not supported), continue
                uint8_t dir_attr_dir_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_DIRECTORY;
                if ( dir_attr_dir_masked == DIR_ENTRY_ATTR_DIRECTORY) {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // This is an in-use 8.3 file, check to see if it matches
                int etf_result = entry_to_filename(dir_entry, filename_wrapper); // Filename + extension of entry at current point in iteration
                if (etf_result != E_SUCCESS)
                {
                    return etf_result;
                }
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
        uint32_t current_cluster_FAT_entry = read_FAT_entry(rca, current_cluster_number);
        if (current_cluster_FAT_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE) {
            return E_FILE_NOT_IN_CWD;
        }
        if (current_cluster_FAT_entry == FAT_ENTRY_DEFECTIVE_CLUSTER) {
            // Fatal error
            __BKPT();
        }
        // FAT entry is in use and points to next cluster
        // Set cluster to the current cluster's FAT entry and continue iteration
        current_cluster_number = current_cluster_FAT_entry;
    }
    myFree(filename_wrapper);
    return E_FILE_NOT_IN_CWD;
}

// Find a file and set *dir_entry to its pointer
int dir_find_file_x(char *filename, uint32_t *entry_sector_bucket, int *entry_number_bucket)
{
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    struct sdhc_card_status my_card_status;
    uint32_t current_cluster_number = cwd;
    // Pointer to hold the pretty filename
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    while (current_cluster_number <= total_data_clusters + 1)
    {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster)
        {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, &my_card_status, sector_data);
            if (read_status != SDHC_SUCCESS)
            {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3 *)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector)
            {
                // Last entry in the directory and it is unused. Return file not found.
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED)
                {
                    myFree(filename_wrapper);
                    return E_FILE_NOT_IN_CWD;
                }
                // Entry not used, continue
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Long dir entry, not supported, continue
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if (dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Entry is a dir (nested directories not supported), continue
                uint8_t dir_attr_dir_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_DIRECTORY;
                if (dir_attr_dir_masked == DIR_ENTRY_ATTR_DIRECTORY)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // This is an in-use 8.3 file, check to see if it matches
                int etf_result = entry_to_filename(dir_entry, filename_wrapper); // Filename + extension of entry at current point in iteration
                if (etf_result != E_SUCCESS)
                {
                    return etf_result;
                }
                int fnamecmp = strncmp((const char *)filename_wrapper->combined, (const char *)filename, (size_t)sizeof(filename));
                if (fnamecmp == 0)
                {
                    *entry_sector_bucket = sector_num;
                    *entry_number_bucket = entry_index;
                    return E_SUCCESS;
                }
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t current_cluster_FAT_entry = read_FAT_entry(rca, current_cluster_number);
        if (current_cluster_FAT_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE)
        {
            return E_FILE_NOT_IN_CWD;
        }
        if (current_cluster_FAT_entry == FAT_ENTRY_DEFECTIVE_CLUSTER)
        {
            // Fatal error
            __BKPT();
        }
        // FAT entry is in use and points to next cluster
        // Set cluster to the current cluster's FAT entry and continue iteration
        current_cluster_number = current_cluster_FAT_entry;
    }
    myFree(filename_wrapper);
    return E_FILE_NOT_IN_CWD;
}

int dir_create_file(char *filename) {
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    struct sdhc_card_status my_card_status;
    uint32_t fcluster;
    // Check if the filename already exists
    int ffr = dir_find_file(filename, &fcluster);
    if (ffr == E_SUCCESS) {
        return E_FILE_EXISTS;
    }
    // Malloc space for the filename wrapper
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    int filename_wrapper_sts = create_filename_wrapper(filename, filename_wrapper);
    if (filename_wrapper_sts != E_SUCCESS) {
        return E_FILE_NAME_INVALID;
    }
    // Find a free entry or return an error
    uint32_t current_cluster_number = cwd;
    while (current_cluster_number <= total_data_clusters + 1)
    {
        // Initialize current sector index to 0
        uint32_t current_sector_index = 0;
        uint32_t first_sector_number = first_sector_of_cluster(current_cluster_number);
    init_sector:;
        uint32_t current_sector_number = first_sector_number + current_sector_index;
        while (current_sector_index <= sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, current_sector_number, &my_card_status, sector_data);
            if (read_status != SDHC_SUCCESS) {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3*)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                // Unused entry found, create file
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED) {
                    // Clear the entry attribute byte
                    dir_entry->DIR_Attr = 0x0;
                    // Set the file size to zero
                    dir_entry->DIR_FileSize = 0x0;
                    // Set the filename
                    strncpy((char *)&dir_entry->DIR_Name[0], (char *)&filename_wrapper->name, 8);
                    strncpy((char *)&dir_entry->DIR_Name[8], (char *)&filename_wrapper->ext, 3);
                    // Set first cluster high and low to zero
                    dir_entry->DIR_FstClusHI = 0;
                    dir_entry->DIR_FstClusLO = 0;
                    // Write the updated sector data to the microSD
                    int write_status = sdhc_write_single_block(rca, current_sector_number, &my_card_status, sector_data);
                    if (write_status != SDHC_SUCCESS)
                    {
                        // Fatal error
                        __BKPT();
                    }
                    return E_SUCCESS;
                }
                // Last entry in sector, and it is unused (no active entries in this sector after this one).
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED) {
                    // If the current entry index != entries per sector
                    // set the name and tag the next entry DIR_ENTRY_LAST_AND_UNUSED
                    if (entry_index+1 != dir_entries_per_sector) {
                        // Clear the entry attribute byte
                        dir_entry->DIR_Attr = 0x0;
                        // Set the file size to zero
                        dir_entry->DIR_FileSize = 0x0;
                        // Set the filename
                        strncpy((char *)&dir_entry->DIR_Name[0], (char *)&filename_wrapper->name, 8);
                        strncpy((char *)&dir_entry->DIR_Name[8], (char *)&filename_wrapper->ext, 3);
                        // Set first cluster high and low to zero
                        dir_entry->DIR_FstClusHI = 0;
                        dir_entry->DIR_FstClusLO = 0;
                        (++dir_entry)->DIR_Name[0] = DIR_ENTRY_LAST_AND_UNUSED;
                        // Write the updated sector data to the microSD
                        int write_status = sdhc_write_single_block(rca, current_sector_number, &my_card_status, sector_data);
                        if (write_status != SDHC_SUCCESS)
                        {
                            // Fatal error
                            __BKPT();
                        }
                        return E_SUCCESS;
                    }
                    // The current entry index == entries per sector
                    else {
                        // Current sector != sectors per cluster, increment the sector and continue checking entries
                        if (current_sector_index+1 < sectors_per_cluster) {
                            current_sector_index++;
                            goto init_sector;
                        }
                        // Current sector == sectors per cluster.
                        // Find a free cluster, or return an error. Unless FSI_Nxt_Free has a valid value, start at cluster 2 per spec
                        uint32_t cluster_search_index = 2;
                        if (FSI_Nxt_Free != FSI_NXT_FREE_UNKNOWN) {
                            cluster_search_index = FSI_Nxt_Free;
                        }
                        // Traverse the FAT and check if there is a free cluster
                        while (cluster_search_index <= total_data_clusters+1) {
                            uint32_t cluster_fat_entry = read_FAT_entry(rca, cluster_search_index);
                            if (cluster_fat_entry == FAT_ENTRY_FREE) {
                                // If there's a free cluster, set the name and tag the first entry in the
                                // next sector DIR_ENTRY_LAST_AND_UNUSED
                                // Clear the entry attribute byte
                                uint32_t sector_num = first_sector_of_cluster(cluster_search_index);
                                int read_status = sdhc_read_single_block(rca, sector_num, &my_card_status, sector_data);
                                if (read_status != SDHC_SUCCESS)
                                {
                                    // Fatal error
                                    __BKPT();
                                }
                                struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3 *)sector_data;
                                dir_entry->DIR_Name[0] = DIR_ENTRY_LAST_AND_UNUSED;
                                dir_entry->DIR_Attr = 0x0;
                                // Set the file size to zero
                                dir_entry->DIR_FileSize = 0x0;
                                // Set the filename
                                strncpy((char *)&dir_entry->DIR_Name[0], (char *)&filename_wrapper->name, 8);
                                strncpy((char *)&dir_entry->DIR_Name[8], (char *)&filename_wrapper->ext, 3);
                                // Set first cluster high and low to zero
                                dir_entry->DIR_FstClusHI = 0;
                                dir_entry->DIR_FstClusLO = 0;
                                (++dir_entry)->DIR_Name[0] = DIR_ENTRY_LAST_AND_UNUSED;
                                // Write the updated sector data to the microSD
                                int write_status = sdhc_write_single_block(rca, current_sector_number, &my_card_status, sector_data);
                                if (write_status != SDHC_SUCCESS)
                                {
                                    // Fatal error
                                    __BKPT();
                                }
                                // Update the FAT
                                write_FAT_entry(rca, cluster_search_index, FAT_ENTRY_ALLOCATED_AND_END_OF_FILE);
                                return E_SUCCESS;
                            }
                            cluster_search_index++;
                        }
                        // All clusters are used, return E_NO_FREE_CLUSTER
                        return E_NO_FREE_CLUSTER;
                    }
                }
                dir_entry++;
                entry_index++;
            }
            current_sector_number++;
            current_sector_index++;
        }
        current_cluster_number++;
    }
    myFree(filename_wrapper);
    return E_NO_FREE_CLUSTER;
}

int dir_delete_file(char *filename) {
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    struct sdhc_card_status my_card_status;
    uint32_t current_cluster_number = cwd;
    // Pointer to hold the pretty filename.
    Filename_8_3_Wrapper *filename_wrapper = myMalloc(sizeof(Filename_8_3_Wrapper));
    while (current_cluster_number <= total_data_clusters + 1) {
        uint32_t sector_num = first_sector_of_cluster(cwd);
        uint32_t sector_index = 0;
        while (sector_index < sectors_per_cluster) {
            uint8_t sector_data[512];
            int read_status = sdhc_read_single_block(rca, sector_num, &my_card_status, sector_data);
            if (read_status != SDHC_SUCCESS)
            {
                // Fatal error
                __BKPT();
            }
            struct dir_entry_8_3 *dir_entry = (struct dir_entry_8_3 *)sector_data;
            int entry_index = 0;
            while (entry_index < dir_entries_per_sector) {
                // Last entry in the directory and it is unused. Return file not found.
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_LAST_AND_UNUSED)
                {
                    myFree(filename_wrapper);
                    return E_FILE_NOT_IN_CWD;
                }
                // Entry not used, continue
                if (dir_entry->DIR_Name[0] == DIR_ENTRY_UNUSED)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Long dir entry, not supported, continue
                uint8_t dir_attr_long_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_LONG_NAME_MASK;
                if (dir_attr_long_masked == DIR_ENTRY_ATTR_LONG_NAME)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // Entry is a dir (nested directories not supported), continue
                uint8_t dir_attr_dir_masked = dir_entry->DIR_Attr & DIR_ENTRY_ATTR_DIRECTORY;
                if (dir_attr_dir_masked == DIR_ENTRY_ATTR_DIRECTORY)
                {
                    dir_entry++;
                    entry_index++;
                    continue;
                }
                // This is an in-use 8.3 file, check to see if it matches
                int etf_result = entry_to_filename(dir_entry, filename_wrapper); // Filename + extension of entry at current point in iteration
                if (etf_result != E_SUCCESS)
                {
                    return etf_result;
                }
                int fnamecmp = strncmp((const char *)filename_wrapper->combined, (const char *)filename, (size_t)sizeof(filename));
                if (fnamecmp == 0) {
                    // File found. Delete it.
                    // But first record the file's first cluster
                    uint32_t file_first_cluster = (uint32_t)dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
                    // Set below values according to deletion process
                    dir_entry->DIR_Name[0] = 0xE5;
                    dir_entry->DIR_FstClusHI = 0x0;
                    // Write the updated sector data to the microSD
                    int write_status = sdhc_write_single_block(rca, sector_num, &my_card_status, sector_data);
                    if (write_status != SDHC_SUCCESS)
                    {
                        // Fatal error
                        __BKPT();
                    }
                    // if the first cluster is 0, it's a new file that's never been written to and the deletion is finished
                    if (file_first_cluster == 0) {
                        return E_SUCCESS;
                    }
                    // Otherwise, traverse the linked list of FAT entries (if linked entries exist) and free all the linked entries
                    uint32_t file_current_fat_entry = file_first_cluster;
                    while (file_current_fat_entry <= total_data_clusters + 1)
                    {
                        uint32_t file_next_fat_entry = read_FAT_entry(rca, file_first_cluster);
                        if (file_next_fat_entry == FAT_ENTRY_DEFECTIVE_CLUSTER || file_current_fat_entry == total_data_clusters + 1)
                        {
                            // Fatal error
                            __BKPT();
                        }
                        if (file_next_fat_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE)
                        {
                            // FAT entry is last and end of file, set it to free and return
                            write_FAT_entry(rca, file_current_fat_entry, FAT_ENTRY_FREE);
                            return E_SUCCESS;
                        }
                        // FAT entry is in use and points to next cluster
                        // Set the current entry to free and continue traversing
                        write_FAT_entry(rca, file_current_fat_entry, FAT_ENTRY_FREE);
                        file_current_fat_entry = file_next_fat_entry;
                    }
                }
                dir_entry++;
                entry_index++;
            }
            sector_num++;
        }
        uint32_t current_cluster_FAT_entry = read_FAT_entry(rca, current_cluster_number);
        if (current_cluster_FAT_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE)
        {
            return E_FILE_NOT_IN_CWD;
        }
        if (current_cluster_FAT_entry == FAT_ENTRY_DEFECTIVE_CLUSTER)
        {
            // Fatal error
            __BKPT();
        }
        // FAT entry is in use and points to next cluster
        // Set cluster to the current cluster's FAT entry and continue iteration
        current_cluster_number = current_cluster_FAT_entry;
    }
    myFree(filename_wrapper);
    return E_FILE_NOT_IN_CWD;
}

int file_open(char *filename, file_descriptor *descrp) {
    // Get an available Stream or return an error
    int get_stream_status = get_available_stream(descrp);
    if (get_stream_status != E_SUCCESS) {
        return get_stream_status;
    }
    struct sdhc_card_status my_card_status;
    // Get the file entry sector and number
    uint32_t file_entry_sector;
    int file_entry_number;
    int get_entry_status = dir_find_file_x(filename, &file_entry_sector, &file_entry_number);
    if (get_entry_status != E_SUCCESS)
    {
        return get_entry_status;
    }
    /**
     * Get the file's directory entry
     */
    uint8_t entry_sector_data[512];
    int entry_read_status = sdhc_read_single_block(rca, file_entry_sector, &my_card_status, entry_sector_data);
    if (entry_read_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    struct dir_entry_8_3 *dir_entry = ((struct dir_entry_8_3 *)entry_sector_data) + file_entry_number;
    uint32_t first_data_cluster = dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
    // If the file is "new" i.e. it has no allocated cluster, skip updating the stream position
    if (first_data_cluster != 0) {
        uint32_t current_cluster_number = first_data_cluster;
        // In my OS data is always appended to the end of a file
        // Get the number of clusters in use via the file size
        uint32_t file_num_clusters = dir_entry->DIR_FileSize / (bytes_per_sector * sectors_per_cluster);
        if (file_num_clusters > 0)
        {
            uint32_t current_cluster_fat_entry = read_FAT_entry(rca, first_data_cluster);
            // Traverse the FAT to take us to the last cluster for this file
            for (int i = 0; i < file_num_clusters; i++)
            {
                uint32_t current_cluster_fat_entry = read_FAT_entry(rca, current_cluster_fat_entry);
            }
            current_cluster_number = current_cluster_fat_entry;
        }
        // Get the number of bytes in-use in the last in-use cluster
        uint32_t bytes_after_cluster_div = dir_entry->DIR_FileSize % (bytes_per_sector * sectors_per_cluster);
        // Get the cluster sector offset
        uint32_t cluster_sector_offset = bytes_after_cluster_div / bytes_per_sector;
        // Get the number of bytes in-use in the last in-use sector
        uint32_t bytes_after_sector_div = bytes_after_cluster_div % bytes_per_sector;
        // Get the first sector of the current cluster
        uint32_t first_sector_current_cluster = first_sector_of_cluster(current_cluster_number);
        // In my OS the "read" position on a newly opened file is always set to the begining of that file
        (currentPCB->streams)[*descrp].position_fgetc = 0;
        // update the position_sector
        (currentPCB->streams)[*descrp].position_sector = first_sector_current_cluster + cluster_sector_offset;
        // Update the stream position_in_sector
        (currentPCB->streams)[*descrp].position_in_sector = bytes_after_sector_div;
    }
    else {
        // the file is new
        // initialize the position_fgetc
        (currentPCB->streams)[*descrp].position_fgetc = 0;
        // initialize the position_sector
        (currentPCB->streams)[*descrp].position_sector = 0;
        // initialize the stream position_in_sector
        (currentPCB->streams)[*descrp].position_in_sector = 0;
    }
    return E_SUCCESS;
}

int file_close(file_descriptor descrp)
{
    // Update the Stream
    (currentPCB->streams)[descrp].position_in_sector = 0;
    (currentPCB->streams)[descrp].position_sector = 0;
    return E_SUCCESS;
}

int file_putbuf(file_descriptor descr, char *bufp, int buflen) {
    struct sdhc_card_status my_card_status;
    // Get the file entry sector and number
    uint32_t file_entry_sector;
    int file_entry_number;
    int get_entry_status = dir_find_file_x(&(currentPCB->streams)[descr].pathname[1], &file_entry_sector, &file_entry_number);
    if (get_entry_status != E_SUCCESS) {
        return get_entry_status;
    }
    /**
     * Get the file's directory entry
     */
    uint8_t entry_sector_data[512];
    int entry_read_status = sdhc_read_single_block(rca, file_entry_sector, &my_card_status, entry_sector_data);
    if (entry_read_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    struct dir_entry_8_3 *dir_entry = ((struct dir_entry_8_3 *)entry_sector_data) + file_entry_number;
    uint32_t first_data_cluster = dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
    /**
     * Allocate the initial cluster if it's a new file
     * Find a free cluster, or return an error. Unless FSI_Nxt_Free has a valid value, start at cluster 2 per spec
     */
    if (first_data_cluster == 0)
    {
        uint32_t cluster_search_index = 2;
        if (FSI_Nxt_Free != FSI_NXT_FREE_UNKNOWN)
        {
            cluster_search_index = FSI_Nxt_Free;
        }
        // Traverse the FAT and check if there is a free cluster
        while (cluster_search_index <= total_data_clusters + 1)
        {
            uint32_t cluster_fat_entry = read_FAT_entry(rca, cluster_search_index);
            if (cluster_fat_entry == FAT_ENTRY_FREE)
            {
                // Update the FAT
                write_FAT_entry(rca, cluster_search_index, FAT_ENTRY_ALLOCATED_AND_END_OF_FILE);
                // Update the stream's cluster number
                first_data_cluster = cluster_search_index;
                break;
            }
            cluster_search_index++;
        }
        if (first_data_cluster == 0) {
            return E_NO_FREE_CLUSTER;
        }
        // Set first cluster high and low
        dir_entry->DIR_FstClusHI = first_data_cluster >> 16;
        dir_entry->DIR_FstClusLO = first_data_cluster & 0xFFFF;
        // Write the updated sector data to the microSD
        int write_status = sdhc_write_single_block(rca, file_entry_sector, &my_card_status, entry_sector_data);
        if (write_status != SDHC_SUCCESS)
        {
            // Fatal error
            __BKPT();
        }
        // Update the stream position_sector
        (currentPCB->streams)[descr].position_sector = first_sector_of_cluster(first_data_cluster);
    }
    // Set the current cluster
    uint32_t current_cluster_number = first_data_cluster;
    // In my OS data is always appended to the end of a file
    // Get the number of clusters in use via the file size
    uint32_t file_num_clusters = dir_entry->DIR_FileSize / (bytes_per_sector * sectors_per_cluster);
    if (file_num_clusters > 0)
    {
        uint32_t current_cluster_fat_entry = read_FAT_entry(rca, first_data_cluster);
        // Traverse the FAT to take us to the last cluster for this file
        for (int i = 0; i < file_num_clusters; i++)
        {
            uint32_t current_cluster_fat_entry = read_FAT_entry(rca, current_cluster_fat_entry);
        }
        current_cluster_number = current_cluster_fat_entry;
    }
    /**
     * Write data to file
     */
    uint32_t write_len = buflen;
    // First check if the amount of data we need to write will spill over into the next sector
    uint32_t sector_start_offset = (currentPCB->streams)[descr].position_in_sector + buflen;
    if (sector_start_offset > bytes_per_sector) {
        // If so call this function recursively to handle the spillover
        // But first check if a new cluster is needed, fml
        // Get the number of bytes in-use in the last in-use cluster
        uint32_t bytes_after_cluster_div = dir_entry->DIR_FileSize % (bytes_per_sector * sectors_per_cluster);
        // Get the sector cluster index to see if we need a new cluster
        uint32_t sector_cluster_index = bytes_after_cluster_div / bytes_per_sector;
        if (sector_cluster_index == sectors_per_cluster-1) {
            // Current sector is the last in the cluster
            // Find a free cluster, or return an error. Unless FSI_Nxt_Free has a valid value, start at cluster 2 per spec
            uint32_t cluster_search_index = 2;
            if (FSI_Nxt_Free != FSI_NXT_FREE_UNKNOWN) {
                cluster_search_index = FSI_Nxt_Free;
            }
            // Traverse the FAT and check if there is a free cluster
            while (cluster_search_index <= total_data_clusters+1) {
                uint32_t cluster_fat_entry = read_FAT_entry(rca, cluster_search_index);
                if (cluster_fat_entry == FAT_ENTRY_FREE) {
                    // Update the FAT
                    write_FAT_entry(rca, current_cluster_number, cluster_search_index);
                    write_FAT_entry(rca, cluster_search_index, FAT_ENTRY_ALLOCATED_AND_END_OF_FILE);
                    current_cluster_number = cluster_search_index;
                }
                else {
                    // Else return an error
                    return E_NO_FREE_CLUSTER;
                }
            }
        }
        write_len = bytes_per_sector - (currentPCB->streams)[descr].position_in_sector;
        char *spillover;
        strncpy((char *)&spillover, &bufp[write_len], buflen - write_len);
        int file_putbuf_status = file_putbuf(descr, spillover, buflen - write_len);
        if (file_putbuf_status != E_SUCCESS) {
            return file_putbuf_status;
        }
    }
    uint8_t position_sector_data[512];
    // Read the current sector data
    int data_read_status = sdhc_read_single_block(rca, (currentPCB->streams)[descr].position_sector, &my_card_status, position_sector_data);
    if (data_read_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    // Update the current sector data at the sector offset with buffer contents
    strncpy((char *)&(position_sector_data[(currentPCB->streams)[descr].position_in_sector]), bufp, buflen);
    int data_write_status = sdhc_write_single_block(rca, (currentPCB->streams)[descr].position_sector, &my_card_status, position_sector_data);
    if (data_write_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    // Update the stream position, subtract 1 to account for the null terminator
    (currentPCB->streams)[descr].position_in_sector = (currentPCB->streams)[descr].position_in_sector + (buflen - 1);
    dir_entry->DIR_FileSize = dir_entry->DIR_FileSize + buflen - 1;
    // Write the updated entry sector data to the microSD
    int filesize_write_status = sdhc_write_single_block(rca, file_entry_sector, &my_card_status, entry_sector_data);
    if (filesize_write_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    return E_SUCCESS;
}

int file_getbuf(file_descriptor descr, char *bufp, int buflen, int *charsreadp) {
    struct sdhc_card_status my_card_status;
    // Get the file entry sector and number
    uint32_t file_entry_sector;
    int file_entry_number;
    int get_entry_status = dir_find_file_x(&(currentPCB->streams)[descr].pathname[1], &file_entry_sector, &file_entry_number);
    if (get_entry_status != E_SUCCESS)
    {
        return get_entry_status;
    }
    /**
     * Get the file's directory entry
     */
    uint8_t entry_sector_data[512];
    int entry_read_status = sdhc_read_single_block(rca, file_entry_sector, &my_card_status, entry_sector_data);
    if (entry_read_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    struct dir_entry_8_3 *dir_entry = ((struct dir_entry_8_3 *)entry_sector_data) + file_entry_number;
    // Get the current cluster based on (currentPCB->streams)[*descr].position_fgetc
    uint32_t first_data_cluster = dir_entry->DIR_FstClusHI << 16 | dir_entry->DIR_FstClusLO;
    uint32_t current_cluster_number = first_data_cluster;
    // In my OS data is always appended to the end of a file
    // Get the number of clusters in use via the file size
    uint32_t pos_num_clusters = (currentPCB->streams)[descr].position_fgetc / (bytes_per_sector * sectors_per_cluster);
    if (pos_num_clusters > 0)
    {
        uint32_t current_cluster_fat_entry = read_FAT_entry(rca, first_data_cluster);
        // Traverse the FAT to take us to the last cluster for this file
        for (int i = 0; i < pos_num_clusters; i++)
        {
            uint32_t current_cluster_fat_entry = read_FAT_entry(rca, current_cluster_fat_entry);
        }
        current_cluster_number = current_cluster_fat_entry;
    }
    uint8_t position_sector_data[512];
    // Get the number of bytes from the current position until the end of the file
    uint32_t remaining_bytes = dir_entry->DIR_FileSize - (currentPCB->streams)[descr].position_fgetc;
    if (remaining_bytes / bytes_per_sector > 0)
    {
        remaining_bytes = remaining_bytes % bytes_per_sector;
    }
    // Read the current sector data and send as many bytes possible to caller
    int data_read_status = sdhc_read_single_block(rca, (currentPCB->streams)[descr].position_sector, &my_card_status, position_sector_data);
    if (data_read_status != SDHC_SUCCESS)
    {
        // Fatal error
        __BKPT();
    }
    // Check if the number of bytes requested spills over into the next sector
    int32_t num_spillover_bytes = buflen - remaining_bytes;
    if (num_spillover_bytes <= 0) {
        num_spillover_bytes = 0;
    }
    // Send as many bytes as you can without spilling over or exceeding requested amount
    strncpy(bufp, (char *)&(position_sector_data[(currentPCB->streams)[descr].position_fgetc]), buflen-num_spillover_bytes);
    *charsreadp = buflen-num_spillover_bytes;
    (currentPCB->streams)[descr].position_fgetc += buflen-num_spillover_bytes;
    if (num_spillover_bytes > 0) {
        // Check if the current sector is the last in the cluster
        // Get the number of bytes in-use in the last in-use cluster
        uint32_t bytes_after_cluster_div = dir_entry->DIR_FileSize % (bytes_per_sector * sectors_per_cluster);
        // Get the sector cluster index to see if we're in the last sector and need to check the FAT
        uint32_t sector_cluster_index = bytes_after_cluster_div / bytes_per_sector;
        if (sector_cluster_index == sectors_per_cluster - 1)
        {
            // This is the last sector in the cluster. 
            // Check if there is data in the next cluster
            uint32_t current_cluster_FAT_entry = read_FAT_entry(rca, current_cluster_number);
            if (current_cluster_FAT_entry >= FAT_ENTRY_ALLOCATED_AND_END_OF_FILE)
            {
                // The next cluster isn't in use, return E_EOF
                return E_EOF;
            }
            else if (current_cluster_FAT_entry == FAT_ENTRY_DEFECTIVE_CLUSTER)
            {
                // Fatal error
                __BKPT();
            }
            // FAT entry is in use and points to next cluster
            // Read the first sector, and send the spillover bytes to the caller
            current_cluster_number = current_cluster_FAT_entry;
            uint32_t position_sector_number = first_sector_of_cluster(current_cluster_number);
            // Read the current sector data
            int data_read_status = sdhc_read_single_block(rca, position_sector_number, &my_card_status, position_sector_data);
            if (data_read_status != SDHC_SUCCESS)
            {
                // Fatal error
                __BKPT();
            }
            // Send the bytes to the callers buffer
            strncpy(bufp, (char *)&position_sector_data, num_spillover_bytes);
            *charsreadp = *charsreadp + num_spillover_bytes;
            (currentPCB->streams)[descr].position_fgetc += num_spillover_bytes;
        }
    }
    return E_SUCCESS;
}
