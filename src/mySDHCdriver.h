/**
 * mySDHCdriver.h
 * A driver for the micro SDHC card
 * 
 * Author: James Nicholson
 */

#ifndef _MYSDHCDRIVER_H
#define _MYSDHCDRIVER_H


#include "microSD.h"

/**
 * Structure to hold additional SDHC status data
 */
extern struct sdhc_card_status *card_status;

/**
 * Micro SDHC mount status
 */
extern int file_structure_mounted;

/**
 * Relative Card Address of currently mounted card.
 */
extern uint32_t rca;

int initSDHC(void);

int file_structure_mount(void);

int file_structure_umount(void);

#endif /* ifndef _MYSDHCDRIVER_H */