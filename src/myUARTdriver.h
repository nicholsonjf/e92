/**
 * myUARTdriver.h
 * A driver for UART IO 
 * 
 * Author: James Nicholson
 */

#ifndef _MYUARTDRIVER_H
#define _MYUARTDRIVER_H

#include "devinio.h"


extern Device UART;

int initUART(void);

#endif /* ifndef _MYUARTDRIVER_H */