/**
 * devinit.h
 * Device initialization
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIT_H
#define _DEVINIT_H

#include "devinio.h"
#include "devinutils.h"

int initDevices(void);

typedef struct
{
    char *pathname;
    Device *device;
} device_p;

extern device_p devices[NUMBER_OF_DEVICES];

#endif /** ifndef _DEVINIT.H **/