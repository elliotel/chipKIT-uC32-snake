/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define TMR2PERIOD ((80000000/256)/10)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big." //reference exercise
#endif

int mytime = 0x5957;
unsigned volatile char* port_E;
unsigned volatile int* IOStatus;
unsigned volatile int* test;
int buttonStatus;
int switchStatus;
int timeoutcount;
int fruitcount;
char score_string[] = "score";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Initializes the IO */
void IOinitialize( void )
{
  IOStatus = (unsigned volatile int*)0xbf886100; //address of TRISE
  *IOStatus &= ~0xff; //sets lower 8 bits to 0 

  TRISD |= 0xfe0; //Sets bits 5-11 of D as input
  TRISF |= 0x2; //Sets bit 1 of F as input

  //Initialize timer 2
  T2CON = 0x70; //set all values to 0 except for the 3 bits in the TCKPS (1110000)
  PR2 = TMR2PERIOD; //Sets the period to the right value
  TMR2 = 0;
  T2CONSET = 0x8000; //Start the timer (15 bit according to manual 14 (timer))
  timeoutcount = 0;
  fruitcount = 0;
  

  port_E = (unsigned volatile char*) 0xbf886110; //address of PORTE 
  return;
}

/* reads input and returns an int indicating which button has been pressed*/
void getinput(struct Snake* s){
    buttonStatus = getbtns();
    if(buttonStatus){
        if (s->player_one) {

            if (buttonStatus & 0x4) {
                s->turnCW = 1;
            }
            if (buttonStatus & 0x8){
                s->turnCCW = 1;
            }
        }
        else {

            if (buttonStatus & 0x1){
                s->turnCW = 1;
            }
            if (buttonStatus & 0x2){
                s->turnCCW = 1;
            }
        }
    }
    
}

void update_game(){
    update_rotation();

    if (IFS(0) & 0x100) { //interrupt flag, found in bit 8 of IFS(0) (lecture), if 1 the timer has reached the period
		IFS(0) &= ~0x100; 
		timeoutcount++;
        fruitcount++;
	}

	if (timeoutcount == 2) { 
		timeoutcount = 0;
		update_board();
        //detect_collition();
        string_to_pixel(1,0,score_string,5);
        //update_score();
		move();
        //display_string(1, score);        
        //display_update();
        
        if(fruitcount == 20){
        fruitcount = 0;
        spawn_fruit();
        }

		tick(&mytime);

		if (*port_E != 255) {
			(*port_E)++;
		}
		else {
			*port_E &= ~0xff;
		}
	}
}
