/**
 * myFAT32driver.h
 * A driver for the FAT32 file system
 * 
 * Author: James Nicholson
 */

#ifndef _MYFAT32DRIVER_H
#define _MYFAT32DRIVER_H

#include "microSD.h"
#include "devinio.h"


Device *FAT32;

int initFAT(void);

/**
 * Number of sectors in a cluster.
 */
extern uint8_t dir_entries_per_sector;

#endif /* ifndef _MYFAT32DRIVER_H */