#include <stdint.h>

static int malloc_initd = 0;

static const int mmagic = 0xdead;

// Create typedef for size of dword
typedef uint64_t dword; 

struct mem_region
{
    uint32_t free : 1;
    uint32_t size : 31;
    uint8_t pid;
    uint8_t data[0];
};

static struct mem_region *mymem;
static struct mem_region *endmymem;

static void malloc_init(void) {
    uint32_t aspacesize = 128 * (1 << 20);
    void *memory = malloc(aspacesize);
    mymem = (struct mem_region*)memory;
    mymem->free = 1;
    mymem->size = aspacesize - sizeof(struct mem_region);
    mymem->pid = 0;
    malloc_initd = 1;
    endmymem = memory + aspacesize;
}

void *myMalloc(uint32_t size) {
    if (malloc_initd == 0) {
        malloc_init();
    }
    struct mem_region *current = mymem;
    while (current < endmymem) {
        if (current->free == 1 && current->size >= size) {
            void *mp = current->data + sizeof(dword) - ((uintptr_t)current->data & (sizeof(dword) - 1));
        }
    }

/*
********** Code from previous approach below, for reference only
********** To be deleted in future commit
*/

struct node {
    // Make unsigned
    int size;
    // Get rid of this. Always go through entire chain of used/free blocks.
    int magic;
    // 
    void *p;
    struct node *next;
};

static struct node freeList;

static void malloc_init(void) {
    void *memory = malloc(128*(1<<20));
    struct node *fnode = (struct node*)memory;
    fnode->p = memory + sizeof (struct node);
    fnode->size = 128 * (1 << 20) - sizeof(struct node);
    fnode->magic = mmagic;
    fnode->next = NULL;
    freeList.next = fnode;
    freeList.size = 0;
    freeList.magic = 0;
    freeList.p = 0;
    malloc_initd = 1;
}

void *myMalloc(unsigned int size) {
    if (malloc_initd == 0) {
        malloc_init();
    }
    struct node *current = freeList.next;
    struct node *previous = &freeList;
    while (current != NULL) {
        // TODO if p is on an 8-byte boundary, skip this operation.
        void *p8 = current->p + 8 - (current->p & 7);
        int usableSize = current->size - (p8 - current->p);
        if (usableSize == size) {
            previous->next = current->next;
            return p8;
        } else if (usableSize > size ) {
            void *next = p8 + usableSize;
            struct node *nextNode = (struct node*)next;
            nextNode->size = current->size - usableSize;
            nextNode->magic = mmagic;
            nextNode->p = next + sizeof(struct node);
            nextNode->next = current->next;
            previous->next = nextNode;
            current->size = usableSize + p8 - current->p;
            return p8;
        }
        current = current->next;
        previous = previous->next;
    }
}

int main (void) {
    
}

void myFree(void *ptr);

int myFreeErrorCode(void *ptr);