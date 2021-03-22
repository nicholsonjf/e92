/**
 * myFAT32driver.h
 * A driver for the FAT32 file system
 * 
 * Author: James Nicholson
 */

#ifndef _MYFAT32DRIVER_H
#define _MYFAT32DRIVER_H

#include <stdint.h>
#include "microSD.h"


/**
 * Structure to hold additional SDHC status data
 */
extern struct sdhc_card_status *card_status;


int initFAT(void);

#endif /* ifndef _MYFAT32DRIVER_H */