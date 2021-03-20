/**
 * myFAT32driver.h
 * A driver for the FAT32 file system
 * 
 * Author: James Nicholson
 */

#include <stdint.h>
#include "microSD.h"


/**
 * Structure to hold additional SDHC status data
 */
extern struct sdhc_card_status *card_status;


int initFAT(void);