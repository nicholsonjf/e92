/**
 * devinio.h
 * Device independent IO
 * 
 * Author: James Nicholson
 */

#include "microSD.h"


uint32_t initFAT(void) {
    uint32_t rca = sdhc_initialize();
    return rca;
}