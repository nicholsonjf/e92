/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

enum modes
{
    O_RDONLY,
    O_WRONLY,
    O_RDWR
};

struct device
{
    int (*fgetc)(char *minor);
    int (*fputc)(char *minor);
    int (*fopen)(char *minor);
    int (*fclose)(char *minor);
};

struct stream
{
    struct device *major;
    char *minor;
    uint32_t directory_sector_number;
    uint8_t uintentry_in_sector; // 0-15
    uint32_t offset;
    uint32_t size;
    uint32_t current_cluster;
};

uint32_t initFAT(void);
