#include "my-malloc.h"
#include "my-malloc-m.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// TODO split into two files
// TODO create ample documentation per instructions in PS
// TODO Explain why you chose best fit alg

struct pcb *currentPCB;

enum error_t
{
    E_SUCCESS = 0,
    E_ADDR_NOT_ALLOCATED = 1,
    E_WRONG_PID = 2,
};

/**
* Implementation Notes
*
* struct mem_region->size does not include sizeof(struct mem_region).
* Additionally, mem_region->size is rounded up to the nearest double word boundary.
* Thus, the location of the next mem_region is: mem_region->data + mem_region->size.
**/

static int malloc_initd = 0;

// Create typedef for size of Dword
typedef uint64_t Dword;

struct mem_region
{
    uint32_t free : 1;
    uint32_t size : 31;
    uint32_t pid;
    uint8_t data[0];
};

static void pcb_init(void) {
    // Check malloc returns non-zero
    // Check return for all system call
    currentPCB = malloc(sizeof(struct pcb));
    currentPCB->pid = 0;
}

static int get_pcb(void) {
    return currentPCB->pid;
}

static struct mem_region *mymem;
static struct mem_region *endmymem;

static void malloc_init(void) {
    pcb_init();
    uint32_t aspacesize = 128 * (1 << 20);
    void *memory = malloc(aspacesize);
    mymem = (struct mem_region*)memory;
    mymem->free = 1;
    mymem->size = aspacesize - sizeof(struct mem_region);
    mymem->pid = get_pcb();
    malloc_initd = 1;
    // So we we can say "while current < endmymem..."
    // In pointer arithmatic, "endmymem" is 128 * (1 << 20) addresses ahead of "memory"
    endmymem = memory + aspacesize;
}

static int qword_boundary(int size)
{
    if ((size & (sizeof(Dword) - 1)) == 0)
    {
        return size;
    }
    return size - (size & (sizeof(Dword) - 1)) + sizeof(Dword);
}

void *myMalloc(uint32_t size) {
    if (malloc_initd == 0) {
        malloc_init();
    }
    size = qword_boundary(size);
    struct mem_region *best = NULL;
    struct mem_region *current = mymem;
    while (current < endmymem) {
        // If we're giving them a pointer to the closest double word boundary that is on or
        // after current->data don't we need to make sure size is lte current->size +
        // distance to next double word boundary?
        if (current->free == 1 && current->size >= size)
        {
            if (best == NULL || current->size < best->size)
            {
                best = current;
            }
        }
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    if (best == NULL) {
        return NULL;
    }
    // Might have to use qword_boundary but probably not.
    void *mp = best->data;
    best->free = 0;
    // Need to split mp if best->size is gt size
    // The below math guarantees that if we split
    // the new_b->size will be gte 1 byte taking
    // into account overhead and padding.
    if (best->size > size + sizeof(struct mem_region)) {
        void *newloc = best->data + size;
        struct mem_region *new_b = (struct mem_region *)newloc;
        new_b->free = 1;
        new_b->size = best->size - size - sizeof(struct mem_region);
        new_b->pid = get_pcb();
        best->size = size;
        printf("Address: %p, Size: %d\n", newloc, new_b->size);
    }
    return mp;
}


int myFreeErrorCode(void *ptr) {
    if (ptr == NULL) {
        return E_ADDR_NOT_ALLOCATED;
    }
    struct mem_region *current = mymem;
    while (current < endmymem)
    {
        if (current->data == ptr && current->free == 0) {
            if (current->pid != get_pcb()) {
                return E_WRONG_PID;
            }
            current->free = 1;
            printf("myFree Current Address: %p, Size: %d\n", current, current->size);
            // TODO merge adjacent free blocks.
            return E_SUCCESS;
        }
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    return E_ADDR_NOT_ALLOCATED;
}

void myFree(void *ptr){
    int rv = myFreeErrorCode(ptr);
    printf("myFree Return Value: %d\n", rv);
}



