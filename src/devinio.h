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

typedef struct device
{
    int (*fgetc)(char *minor);
    int (*fputc)(char *minor);
} Device;

struct stream
{
    struct Device *major;
    char *minor;
    uint32_t directory_sector_number;
    uint8_t uintentry_in_sector; // 0-15
    uint32_t offset;
    uint32_t size;
    uint32_t current_cluster;
};

int initDevIO(void);
