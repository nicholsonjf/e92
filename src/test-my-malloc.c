#include "my-malloc.h"
#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void stress_test(void) {
    void *blocks[1000];
    for (int j = 0; j < 1000; j++)
    {
        blocks[j] = NULL;
    }
    srandom(time(NULL));
    for (int i = 0; i < 5000; i++)
    {
        if (random() % 2 == 0)
        {
            for (int j = 0; j < 1000; j++)
            {
                if (blocks[j] == NULL)
                {
                    blocks[j] = myMalloc(random() % 1000);
                    break;
                }
            }
        }
        else
        {
            for (int j = 0; j < 1000; j++)
            {
                if (blocks[j] != NULL)
                {
                    myFree(blocks[j]);
                    blocks[j] = NULL;
                }
            }
        }
    }
}

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

int main(void)
{
}