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
};

uint32_t initFAT(void);