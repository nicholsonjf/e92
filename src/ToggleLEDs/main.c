/**
 * main.c
 * ToggleLED project main program
 *
 * Demonstrates using pushbuttons to toggle the LEDs
 * 
 */


#include "delay.h"
#include "led.h"
#include "pushbutton.h"

int main(void) {
	
	const unsigned long int delayCount = 0x7ffff;
	
	/* Initialize all of the LEDs */
	ledInitAll();
	/* Initialize both of the pushbuttons */
	pushbuttonInitAll();
	
	int startLED = 0;

	while(1) {
		int activeLED = startLED;
		while(activeLED <= 3){
			if(activeLED == 0) {
				ledOrangeOn();
			}
			if(activeLED == 1) {
				ledYellowOn();
			}
			if(activeLED == 2) {
				ledGreenOn();
			}
			if(activeLED == 3) {
				ledBlueOn();
			}
			if(sw1In()) {
				if(activeLED == 0){
					ledOrangeOff();	
				}
				if(activeLED == 1){
					ledYellowOff();	
				}
				if(activeLED == 2){
					ledGreenOff();	
				}
				if(activeLED == 3){
					ledBlueOff();	
				}
				delay(delayCount);
				activeLED++;
			}
		}	
	}
	
	return 0;
}