#include <stdlib.h>

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