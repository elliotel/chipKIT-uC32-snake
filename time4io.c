#include <stdint.h>  
#include <pic32mx.h>  
#include "mipslab.h"  

int getsw(void) {
	return (int) (PORTD >> 8) & 0xf; //returns the status of the input in bits 11 to 8 (SW4 to SW1, with SW1 as the LSB)
}

int getbtns(void) {
	return (int) (((PORTD & 0xe0) >> 4) | ((PORTF & 0x2) >> 1)); //returns the status of the input in bits 7 to 5 (BTN4 to BTN2, with BTN 2 as the LSB)
}