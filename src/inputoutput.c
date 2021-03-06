/* inputoutput.c
  (based on mipslabwork.c)

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING
   
   This file modified 2021-12-14 by Elliot Elmenbeck and Isak Karlander */

#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include "snake.h" /* Declatations for these labs */



#define TMR2PERIOD ((80000000 / 256) / 10)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big." //reference exercise
#endif

unsigned volatile int *IOStatus;
int timeoutcount;
int fruitcount;

/* Interrupt Service Routine */
void user_isr(void) { return; }

/* Initializes the IO */
void IOinitialize(void) {
  IOStatus = (unsigned volatile int *)0xbf886100; // address of TRISE
  *IOStatus &= ~0xff;                             // sets lower 8 bits to 0

  TRISD |= 0xfe0; // Sets bits 5-11 of D as input
  TRISF |= 0x2;   // Sets bit 1 of F as input

  // Initialize timer 2
  T2CON =
      0x70; // set all values to 0 except for the 3 bits in the TCKPS (1110000)
      PR2 = TMR2PERIOD; // Sets the period to the right value
  TMR2 = 0;
  T2CONSET = 0x8000; // Start the timer (15 bit according to manual 14 (timer))
  timeoutcount = 0;
  fruitcount = 0;
  return;
}

//Checks if any buttons are being pressed
int get_btns(void) {
	return (int) (((PORTD & 0xe0) >> 4) | ((PORTF & 0x2) >> 1)); 
}

/* Reads input and returns an int indicating which button has been pressed*/
void get_input(struct Snake *s) {
  int button_status = get_btns();
  if (button_status) {
    if (s->player_one && !(s->ai.enabled)) {

      if (button_status & 0x4) {
        s->turn_CW = 1;
      }
      if (button_status & 0x8) {
        s->turn_CCW = 1;
      }
    } else if (!(s->ai.enabled)) {

      if (button_status & 0x1) {
        s->turn_CW = 1;
      }
      if (button_status & 0x2) {
        s->turn_CCW = 1;
      }
    }
  }
}

//Updates the game, this function is run continously while a game is in progress
_Bool update_game() {
  update_rotation();

  if (IFS(0) & 0x100) { // interrupt flag, found in bit 8 of IFS(0) (lecture),
                        // if 1 the timer has reached the period
    IFS(0) &= ~0x100;
    timeoutcount++;
    fruitcount++;
  }

  if (timeoutcount == 2) {
    timeoutcount = 0;
    update_screen();
    //Moves both snakes, and handles all calculations regarding new positions, collisions and so on.
    if (move()) {
      // Game ended
      return 1;
    }

    //Counter for how often fruits spawn, lower = more frequent
    if (fruitcount == 20) {
      fruitcount = 0;
      spawn_fruit();
    }
  }
  return 0;
}
