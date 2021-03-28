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

Device LEDGreen;
Device LEDYellow;
Device LEDBlue;
Device LEDOrange;

#endif /* ifndef _MYLEDDRIVER_H */