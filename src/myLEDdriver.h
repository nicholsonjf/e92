/**
 * myLEDdriver.h
 * A driver for the LED lights
 * 
 * Author: James Nicholson
 */

#ifndef _MYLEDDRIVER_H
#define _MYLEDDRIVER_H

#include "devinio.h"

int initLED(void);

extern Device LEDGreen;
extern Device LEDYellow;
extern Device LEDBlue;
extern Device LEDOrange;

#endif /* ifndef _MYLEDDRIVER_H */