#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#include <stdlib.h>

extern struct mem_region *mymem;
extern struct mem_region *endmymem;

struct mem_region
{
    uint32_t free : 1;
    uint32_t size : 31;
    uint32_t pid;
    uint8_t data[0];
};

void *myMalloc(uint32_t size);
void myFree(void *ptr);
void memoryMap(void);
int myFreeErrorCode(void *ptr);
int myMemset(void *p, uint8_t val, long len);
int myMemchk(void *p, uint8_t val, long len);

#endif /* ifndef _MYMALLOC_H */ 