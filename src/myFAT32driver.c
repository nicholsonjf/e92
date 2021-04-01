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

Device FAT32;

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
    int fatfclose_status = file_close(*fd);
    if (fatfclose_status != E_SUCCESS)
    {
        return fatfclose_status;
    }
    return E_SUCCESS;
}

int fatfopen(char *pathname, file_descriptor *fd)
{
    // remove leading slash to get the filename
    char *filename = ++pathname;
    int fatfopen_status = file_open(filename, fd);
    if (fatfopen_status != E_SUCCESS) {
        return fatfopen_status;
    }
    return E_SUCCESS;
}

int fatfcreate(char *pathname) {
    // remove leading slash to get the filename
    char * filename = ++pathname;
    size_t pathname_len = strlen(filename);
    if (pathname_len < 1 || pathname_len > 11)
    {
        return E_FILE_NAME_INVALID;
    }
    int create_file_status = dir_create_file(filename);
    if (create_file_status != E_SUCCESS)
    {
        return create_file_status;
    }
    return E_SUCCESS;
}

int fatfdelete(char *pathname)
{
    // remove leading slash
    int delete_file = dir_delete_file(++pathname);
    if (delete_file != E_SUCCESS)
    {
        return delete_file;
    }
    return E_SUCCESS;
}


int initFAT(void) {
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
    FAT32.fdelete = fatfdelete;
    FAT32.fcreate = fatfcreate;
    return E_SUCCESS;
}