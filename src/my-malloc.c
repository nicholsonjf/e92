#include "my-malloc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "pcb.h"
#include "sdram.h"

struct pcb *currentPCB;

static void pcb_init(void)
{
    // Check malloc returns non-zero
    // Check return for all system call
    currentPCB = malloc(sizeof(struct pcb));
    currentPCB->pid = 0;
    // initialize streams to not in use
    // Check for the first open Stream in pcb->streams
    for (int i = 0; i < sizeof(currentPCB->streams) / sizeof(currentPCB->streams[0]); i++)
    {
        (currentPCB->streams)[i].in_use = 0;
    }
}

/**
* Implementation Notes
* 
* I chose the best fit algorithm because it made intuitive sense to me, and
* seeming relatively straight-forward to implement. Since I've never implemented
* and of the memory allocation algorithms before, I wanted to choose one that I
* could wrap my head around and wasn't overly complex to implement.
*
* struct mem_region->size does not include sizeof(struct mem_region).
* Additionally, mem_region->size is rounded up to the nearest double word boundary.
* Thus, the location of the next mem_region is: mem_region->data + mem_region->size.
**/

static int malloc_initd = 0;

// Create typedef for size of Dword
typedef uint64_t Dword;

struct mem_region *mymem;
struct mem_region *endmymem;

static int get_pcb(void) {
    return currentPCB->pid;
}

static void malloc_init(void) {
    pcb_init();
    const void *memory_start = (const void*)SDRAM_START;
    mymem = (struct mem_region*)memory_start;
    mymem->free = 1;
    mymem->size = SDRAM_SIZE - sizeof(struct mem_region);
    mymem->pid = get_pcb();
    malloc_initd = 1;
    const void *memory_end = (const void*)SDRAM_END;
    endmymem = (struct mem_region*)memory_end;
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
    if (size < 1) {
        return NULL;
    }
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
    }
    return mp;
}

int myFreeErrorCode(void *ptr) {
    if (malloc_initd == 0)
    {
        return E_ADDR_NOT_ALLOCATED;
    }
    if (ptr == NULL) {
        return E_ADDR_NOT_ALLOCATED;
    }
    struct mem_region *previous = NULL;
    struct mem_region *current = mymem;
    while (current < endmymem)
    {
        if (current->data == ptr && current->free == 0) {
            if (current->pid != get_pcb()) {
                return E_WRONG_PID;
            }
            current->free = 1;
            if (previous != NULL && previous->free == 1) {
                previous->size = previous->size + current->size + sizeof(struct mem_region);
                current = previous;
            }
            struct mem_region *next = (void *)current + current->size + sizeof(struct mem_region);
            if (next < endmymem && next->free == 1) {
                current->size = current->size + next->size + sizeof(struct mem_region);
            }

            return E_SUCCESS;
        }
        previous = current;
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    return E_ADDR_NOT_ALLOCATED;
}


int myFree(void *ptr) {
    int myFreeStatus = myFreeErrorCode(ptr);
    if (myFreeStatus != E_SUCCESS) {
    	return myFreeStatus;
    }
    return E_SUCCESS;
}


void memoryMap(void) {
    if (malloc_initd == 0)
    {
        malloc_init();
    }
    struct mem_region *current = mymem;
    myprintf("\n");
    myprintf("%10s%5s%7s%12s\n", "Address", "PID", "Free", "Size");
    while (current < endmymem)
    {
        char **free;
        if (current->free == 0){
            char *false = "false";
            free = &false;
        } else {
            char *true = "true";
            free = &true;
        }
        myprintf("%10p%5d%7s%12d\n", current->data, current->pid, *free, current->size);
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    myprintf("\n");
}

int myMemset(void *p, uint8_t val, long len)
{
    if (malloc_initd == 0)
    {
        return E_ADDR_NOT_ALLOCATED;
    }
    struct mem_region *current = mymem;
    while (current < endmymem)
    {
        // Convert start pointer to an unsigned long so we can do integer math.
        long start_addr = (long)p;
        if ((void*)current->data <= p && (void*)(start_addr + len) <= (void*)(current->data + current->size))
        {
            memset(p, val, len);
            return E_SUCCESS;
        }
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    return E_ADDR_SPC;
}

int myMemchk(void *p, uint8_t val, long len)
{
    if (malloc_initd == 0)
    {
        return E_ADDR_NOT_ALLOCATED;
    }
    int chkstatus = 0;
    struct mem_region *current = mymem;
    while (current < endmymem)
    {
        // Convert start pointer to an unsigned long so we can do integer math.
        long start_addr = (long)p;
        if ((void *)current->data <= p && (void *)(start_addr + len) <= (void *)(current->data + current->size))
        {
            chkstatus = 1;
            long current_byte = start_addr;
            while (current_byte < start_addr + len)
            {
                unsigned char byte = *((unsigned char *)current_byte);
                if ((uint8_t)byte != val) {
                    chkstatus = 0;
                    break;
                }
                current_byte++;
            }
            break;
        }
        current = (void *)current + current->size + sizeof(struct mem_region);
    }
    if (chkstatus == 0) {
        myprintf("%s\n", "memchk failed");
    } else {
        myprintf("%s\n", "memchk succeeded");
    }
    return E_SUCCESS;
}
