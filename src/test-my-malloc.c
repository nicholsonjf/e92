#include "my-malloc.h"
#include "pcb.h"
#include <stdio.h>

int main(void)
{
    void *mm = myMalloc(1234);
    printf("mm Address: %p\n", mm);
    void *nn = myMalloc(89898);
    printf("nn Address: %p\n", nn);
    currentPCB->pid = 0;
    myFree(mm);
    void *kk = myMalloc(1234);
    printf("kk Address: %p\n", kk);
    memoryMap();
}