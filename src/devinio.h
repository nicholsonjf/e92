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
    int (*fputc)(file_descriptor *fd, char *bufp, int buflen);
    int (*fopen)(char *pathname, file_descriptor *fd);
    int (*fdelete)(char *pathname);
    int (*fclose)(file_descriptor *fd);
    int (*fcreate)(char *pathname);
} Device;

typedef struct stream
{
    Device *device; // pointer to the Device used to operate on the file
    uint8_t in_use; // whether the stream is currently in use (stream.in_use=1) or not (stream.in_use=0)
    char pathname[12]; // the pathname of the file
    // FAT32 members
    uint32_t first_cluster; // the first data cluster for this file
    uint32_t position_sector; // the sector number of the open file's position
    uint32_t position_in_sector; // the offset in bytes within the position_sector
} Stream;

int myfclose(file_descriptor *fd);

int myfopen(char *pathname, file_descriptor *fd);

int myfdelete(char *pathname);

int myfcreate(char *pathname);

int myfputc(file_descriptor *fd, char *bufp, int buflen);

#endif /* ifndef _DEVINIO_H */
