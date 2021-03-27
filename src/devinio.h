/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIO_H
#define _DEVINIO_H

#include <stdint.h>

/**
 * A file_descriptor is used as an index into an array of structures describing
 * open files
 * All entries in the array are initially closed and not associated with any
 * open files
 */
typedef uint32_t file_descriptor;

typedef struct device
{
    int (*fgetc)(char *minor);
    int (*fputc)(char *minor);
    int (*fopen)(char *pathname, file_descriptor *fd);
    int (*fclose)(void);
} Device;

typedef struct stream
{
    Device *device;
    uint8_t status;
    char *pathname;
    // FAT32 members
    uint32_t block;
    uint8_t entry_in_sector; // 0-15
    uint32_t offset;
    uint32_t size;
    uint32_t current_cluster;

} Stream;

int myfopen(char *pathname, file_descriptor *fd);

int myfdelete(char *pathname);

#endif /* ifndef _DEVINIO_H */