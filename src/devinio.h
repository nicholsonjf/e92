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
};

struct stream
{
    struct device *major;
    char *minor;
    directory_sector_number
    entry_in_sector 0-15
    position
    size
    current_cluster
};

uint32_t initFAT(void);