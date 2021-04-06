/**
 * myPBdriver.h
 * A driver for the K70 puh buttons
 * 
 * Author: James Nicholson
 */

#ifndef _MYPBDRIVER_H
#define _MYPBDRIVER_H

#include "devinio.h"

int initPB(void);

extern Device SW1;
extern Device SW2;

#endif /* ifndef _MYPBDRIVER_H */