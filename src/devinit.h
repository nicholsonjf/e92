/**
 * devinit.h
 * Device initialization
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIT_H
#define _DEVINIT_H

#include "devinio.h"

int initDevices(void);

typedef struct device_p
{
    char *pathname;
    Device *device;
} device_p;

extern device_p *devices;

#endif /** ifndef _DEVINIT.H **/