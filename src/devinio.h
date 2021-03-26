/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIO_H
#define _DEVINIO_H

typedef uint32_t file_descriptor;

typedef struct device
{
    int (*fgetc)(char *minor);
    int (*fputc)(char *minor);
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

int initDevIO(void);

int fopen(char *pathname, file_descriptor *fd);

#endif /* ifndef _DEVINIO_H */