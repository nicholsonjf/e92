#include <stdint.h>
#include <stdio.h>

static int malloc_initd = 0;

static const int mmagic = 0xdead;

// Create typedef for size of dword
typedef uint64_t dword;

struct pcb
{
    int pid; 
};

// Call myMalloc in main to allocate 
static struct pcb *currentPCB; 

struct mem_region
{
    uint32_t free : 1;
    uint32_t size : 31;
    uint32_t pid;
    uint8_t data[0];
};

static void initial_pcb(void) {
    // Check malloc returns non-zero
    // Check return ffor all system call
    currentPCB = malloc(sizeof(struct pcb));
    currentPCB->pid = 0;
}

static int get_pcb(void) {
    return &currentPCB->pid;
}

static struct mem_region *mymem;
static struct mem_region *endmymem;

static void malloc_init(void) {
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

void *myMalloc(uint32_t size) {
    if (malloc_initd == 0) {
        malloc_init();
    }
    struct mem_region *best = NULL;
    struct mem_region *current = mymem;
    while (current < endmymem) {
        // If we're giving them a pointer to the closest double word boundary that is on or
        // after current->data don't we need to make sure size is lte current->size +
        // distance to next double word boundary?
        if (current->free == 1 && current->size >= size) {
            if (best == NULL || current < best)
            {
                best = current;
            }
        }
    }
    if (best == NULL) {
        return NULL;
    }
    // Don't fully understand how this math works.
    void *mp = best->data + sizeof(dword) - ((uintptr_t)best->data & (sizeof(dword) - 1));
    // Need to split mp if best->size is gt size
    return mp;
}


void myFree(void *ptr);

int myFreeErrorCode(void *ptr);

int main(void)
{
}



