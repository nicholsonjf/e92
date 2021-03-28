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
#include <string.h>

/**
 * Initialize a structure to hold additional SDHC status data, defined in initFAT()
 */
struct sdhc_card_status *my_card_status;

uint8_t dir_entries_per_sector;


int fatfgetc(file_descriptor *fd)
{
    return E_SUCCESS;
}

int fatfputc(char c, file_descriptor *fd)
{
    return E_SUCCESS;
}

int fatfclose(file_descriptor *fd)
{
    return E_SUCCESS;
}

int fatfopen(char *pathname, file_descriptor *fd)
{
    int fatfopen_status = file_open(pathname, fd);
    if (fatfopen_status != E_SUCCESS) {
        return fatfopen_status;
    }
    return E_SUCCESS;
}

int fatfcreate(char *pathname, file_descriptor *fd) {
    return E_SUCCESS;
}

int fatfdelete(char *pathname)
{
    if (pathname[0] != '/')
    {
        return E_FILE_NAME_INVALID;
    }
    size_t pathname_len = strlen(&pathname[1]);
    if (pathname_len < 1 || pathname_len > 11)
    {
        return E_FILE_NAME_TOO_LONG;
    }
    int delete_file = dir_delete_file(&pathname[1]);
    if (delete_file != E_SUCCESS)
    {
        return delete_file;
    }
    return E_SUCCESS;
}


int initFAT(void) {
    uint8_t dir_entries_per_sector = bytes_per_sector / sizeof(struct dir_entry_8_3);
    // Set cwd to the root cluster
    int set_cwd = dir_set_cwd_to_root();
    if (set_cwd != E_SUCCESS) {
        return set_cwd;
    }
    // Define the struct Device FAT32
    FAT32.fgetc = fatfgetc;
    FAT32.fputc = fatfputc;
    FAT32.fclose = fatfclose;
    FAT32.fopen = fatfopen;
    return E_SUCCESS;
}