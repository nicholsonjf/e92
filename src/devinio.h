/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIO_H
#define _DEVINIO_H

enum modes
{
    O_RDONLY,
    O_WRONLY,
    O_RDWR
};

typedef struct device
{
    int (*fgetc)(char *minor);
    int (*fputc)(char *minor);
    int (*fclose)(char *minor);
} Device;

typedef struct stream
{
    Device *device;
    uint8_t flag;
    // FAT32 members
    char *minor;
    uint32_t block;
    uint8_t uintentry_in_sector; // 0-15
    uint32_t offset;
    uint32_t size;
    uint32_t current_cluster;
    // Pushbutton members

} Stream;

int initDevIO(void);

int fopen(char *pathname, file_descriptor *fd);

#endif /* ifndef _DEVINIO_H */