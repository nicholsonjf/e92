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
    int (*fgetc)(file_descriptor *fd);
    int (*fputc)(char c, file_descriptor *fd);
    int (*fopen)(char *pathname, file_descriptor *fd);
    int (*fdelete)(char *pathname);
    int (*fclose)(file_descriptor *fd);
    int (*fcreate)(char *pathname, file_descriptor *fd);
} Device;

typedef struct stream
{
    Device *device; // pointer to the Device used to operate on the file
    uint8_t in_use; // whether the stream is currently in use (stream.in_use=1) or not (stream.in_use=0)
    char *pathname; // the pathname of the file
    // FAT32 members
    uint32_t position; // current file position in bytes
    uint32_t first_cluster; // cluster number of the file's first data cluster
    uint32_t first_sector; // sector number of the first sector in first_cluster
} Stream;

int myfopen(char *pathname, file_descriptor *fd);

int myfdelete(char *pathname);

#endif /* ifndef _DEVINIO_H */