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


extern Device FAT32;

int initFAT(void);

#endif /* ifndef _MYFAT32DRIVER_H */