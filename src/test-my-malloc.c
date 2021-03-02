#include "my-malloc.h"
#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Randomly mallocs and frees 500 blocks to test the overal function of the two functions.
void stress_test(void) {
    void *blocks[100];
    for (int j = 0; j < 100; j++)
    {
        blocks[j] = NULL;
    }
    srandom(time(NULL));
    for (int i = 0; i < 500; i++)
    {
        if (i % 10 == 0) {
            memoryMap();
        }
        if (random() % 2 == 0)
        {
            for (int j = 0; j < 100; j++)
            {
                if (blocks[j] == NULL)
                {
                    blocks[j] = myMalloc(random() % 100);
                    fprintf(stdout, "Malloc'd address: %p\n", blocks[j]);
                    break;
                }
            }
        }
        else
        {
            for (int j = 0; j < 100; j++)
            {
                if (blocks[j] != NULL)
                {
                    myFree(blocks[j]);
                    fprintf(stdout, "Free'd address: %p\n", blocks[j]);
                    blocks[j] = NULL;
                }
            }
        }
    }
}

// Demonstrates that free will merge adjacent free blocks.
void merge_test(void) {
    memoryMap();
    fprintf(stdout, "\n");
    void *mm = myMalloc(1234);
    memoryMap();
    fprintf(stdout, "\n");
    void *nn = myMalloc(89898);
    memoryMap();
    fprintf(stdout, "\n");
    void *kk = myMalloc(1234);
    memoryMap();
    fprintf(stdout, "\n");
    myFree(mm);
    fprintf(stdout, "\n");
    memoryMap();
    fprintf(stdout, "\n");
    myFree(kk);
    fprintf(stdout, "\n");
    memoryMap();
    fprintf(stdout, "\n");
    myFree(nn);
    fprintf(stdout, "\n");
    memoryMap();
}

// Mallocs a block, sets some bytes to a value, and checks they equal the value.
void test_mem_chk(void) {
    void *mm = myMalloc(1234);
    int ms = myMemset(mm, (uint8_t)89, (long)100);
    int mc = myMemchk(mm, (uint8_t)89, (long)100);
}

void test_best_fit(void) {
    memoryMap();
    // Malloc 5 blocks
    void *a = myMalloc(500);
    void *b = myMalloc(5000);\
    void *c = myMalloc(1500);
    void *d = myMalloc(50);
    void *e = myMalloc(10000);
    memoryMap();
    // Free two holes
    myFree(b);
    myFree(d);
    memoryMap();
    // Malloc an amount less than the first free block
    void *f = myMalloc(49);
    // Malloc will use best fit and allocate the size 50 block
    memoryMap();
}

int main(void)
{
    fprintf(stdout, "%s\n", "******START STRESS TEST***");
    stress_test();
    fprintf(stdout, "%s\n", "******END STRESS TEST***");
    fprintf(stdout, "%s\n", "******START MERGE TEST***");
    merge_test();
    fprintf(stdout, "%s\n", "******END MERGE TEST***");
    fprintf(stdout, "%s\n", "******START MEMSET/MEMCHK TEST***");
    test_mem_chk();
    fprintf(stdout, "%s\n", "******END MEMSET/MEMCHK TEST***");
    fprintf(stdout, "%s\n", "******START BEST FIT TEST***");
    test_best_fit();
    fprintf(stdout, "%s\n", "******END BEST FIT TEST***");
}