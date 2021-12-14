/* boardmanager.c 
  (based on mipslabfunc.c)
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING 
   
   This file modified 2021-12-14 by Elliot Elmenbeck and Isak Karlander */

#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include "snake.h" /* Declatations for these labs */


/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
  int i;
  for (i = cyc; i > 0; i--)
    ;
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick(unsigned int *timep) {
  /* Get current value, store locally */
  register unsigned int t = *timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if ((t & 0x0000000f) >= 0x0000000a)
    t += 0x00000006;
  if ((t & 0x000000f0) >= 0x00000060)
    t += 0x000000a0;
  /* Seconds are now OK */

  if ((t & 0x00000f00) >= 0x00000a00)
    t += 0x00000600;
  if ((t & 0x0000f000) >= 0x00006000)
    t += 0x0000a000;
  /* Minutes are now OK */

  if ((t & 0x000f0000) >= 0x000a0000)
    t += 0x00060000;
  if ((t & 0x00ff0000) >= 0x00240000)
    t += 0x00dc0000;
  /* Hours are now OK */

  if ((t & 0x0f000000) >= 0x0a000000)
    t += 0x06000000;
  if ((t & 0xf0000000) >= 0xa0000000)
    t = 0;
  /* Days are now OK */

  *timep = t; /* Store new value */
}

uint8_t spi_send_recv(uint8_t data) {
  while (!(SPI2STAT & 0x08))
    ;
  SPI2BUF = data;
  while (!(SPI2STAT & 1))
    ;
  return SPI2BUF;
}

void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
  quicksleep(10);
  DISPLAY_ACTIVATE_VDD;
  quicksleep(1000000);

  spi_send_recv(0xAE);
  DISPLAY_ACTIVATE_RESET;
  quicksleep(10);
  DISPLAY_DO_NOT_RESET;
  quicksleep(10);

  spi_send_recv(0x8D);
  spi_send_recv(0x14);

  spi_send_recv(0xD9);
  spi_send_recv(0xF1);

  DISPLAY_ACTIVATE_VBAT;
  quicksleep(10000000);

  spi_send_recv(0xA1);
  spi_send_recv(0xC8);

  spi_send_recv(0xDA);
  spi_send_recv(0x20);

  spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
  int i;
  if (line < 0 || line >= 4)
    return;
  if (!s)
    return;

  for (i = 0; i < 16; i++)
    if (*s) {
      textbuffer[line][i] = *s;
      s++;
    } else
      textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
  int i, j;

  for (i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;

    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(x & 0xF);
    spi_send_recv(0x10 | ((x >> 4) & 0xF));

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 32; j++)
      spi_send_recv(~data[i * 32 + j]);
  }
}

void display_screen(int x, const uint8_t *data) {
  int i, j;

  for (i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;

    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(x & 0xF);
    spi_send_recv(0x10 | ((x >> 4) & 0xF));

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 128; j++)
      spi_send_recv(~data[i * 128 + j]);
  }
}

//Sets up the game board
void set_up_board(char map) {
  int amount = 0;
  int donut_amount = 0;
  int churro_amount = 0;

  //Draws the corners of the board
  int x;
  int y;
  for (x = 0; x < 128; x++) {
    for (y = 0; y < 32; y++) {
      if (((y == 0 || y == 31) && x >= 27) || x == 27 || x == 127) {
        board[x][y] = 1;
      } else {
        board[x][y] = 0;
      }
    }
  }

  //Depending on the difficulty level, the amount of obstacles change
  switch (map) {
  case 'e':
    update_screen();
    return;
    break;
  case 'm':
    amount = 8;
    break;
  case 'h':
    amount = 16;
    break;
  }
  
  //Randomizes the amount of donut and churro obstacles based on the amount of total obstacles
  donut_amount = (rand() % (amount - 1)) + 1;
  churro_amount = amount - donut_amount;
  struct Coordinate coords[amount];
  int i, j, k, l;
  int x_temp;
  int y_temp;
  _Bool success;

  for (i = 0; i < donut_amount; i++) {
    success = 0;
    while (!success) {
      x_temp = (rand() % 99) + 27;
      y_temp = (rand() % 29) + 1;
      success = 1;
      for (k = x_temp; k < (x_temp + 3); k++) {
        for (l = y_temp; l < (y_temp + 3); l++) {
          //Evaluates if there exists something on this spot, if so, loops around and finds a new spot
          if (board[k][l] == 1 || (k < 60 && l < 10) || (k > 100 && l > 21)) {
            success = 0;
          }
        }
      }
      if (success) {
        //Draws the "donut"
        for (k = x_temp; k < (x_temp + 3); k++) {
          for (l = y_temp; l < (y_temp + 3); l++) {
            if (k != x_temp + 1 || l != y_temp + 1) {
              board[k][l] = 1;
            }
          }
        }
      }
    }
  }
  for (j = 0; j < churro_amount; j++) {
    success = 0;
    while (!success) {
      x_temp = (rand() % 99) + 27;
      y_temp = (rand() % 29) + 1;
      success = 1;
      for (k = x_temp; k < (x_temp + 3); k++) {
        for (l = y_temp; l < (y_temp + 5); l++) {
          //Evaluates if there exists something on this spot, if so, loops around and finds a new spot
          if (board[k][l] == 1 || (k < 50 && l < 10) || (k > 110 && l > 21)) {
            success = 0;
          }
        }
      }
      if (success) {
        //Draws the "churro"
        for (k = x_temp; k < (x_temp + 3); k++) {
          for (l = y_temp; l < (y_temp + 5); l++) {
            if (!(k == x_temp + 1 &&
                  (l == y_temp + 1 || l == y_temp + 2 || l == y_temp + 3))) {
              board[k][l] = 1;
            }
          }
        }
      }
    }
  }
  update_screen();
}

void set_up_score(void) {
  string_to_pixel(1, 0, "P1:", 3);
  string_to_pixel(1, 16, "P2:", 3);
  update_score();
}

void display_update(void) {
  int i, j, k;
  int c;
  for (i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 16; j++) {
      c = textbuffer[i][j];
      if (c & 0x80)
        continue;

      for (k = 0; k < 8; k++)
        spi_send_recv(font[c * 8 + k]);
    }
  }
}

// Converts a string into pixels at a specified location on the screen
void string_to_pixel(int x, int y, char *s, int l) {
  int c, i, row;
  int x_new, y_new;
  _Bool chara = 0;
  // loops for every character in the array
  for (c = 0, x_new = x; c < l; c++, x_new++, s++) {
    chara = 0;
    for (i = 0; i < 8; i++) {
      row = font[(*s) * 8 + i];
      if (row != 0) {
        // converts each set of 8 bits into pixels
        for (y_new = y; y_new < y + 8; y_new++) {
          if (row & 0x1)
            board[x_new][y_new] = 1;
          row = row >> 1;
        }
        x_new++;
        chara = 1;
      }
    }
    if (!chara)
      x_new += 4;
  }
}

void clear_score_display() {
  int i, j;
  for (i = 0; i < 27; i++) {
    for (j = 8; j < 16; j++) {
      board[i][j] = 0;
    }
  }

  for (i = 0; i < 27; i++) {
    for (j = 24; j < 32; j++) {
      board[i][j] = 0;
    }
  }
}

// converts coordinates to pixels
void update_screen(void) {

  int y = 0;
  int c = 0;
  int x;
  int b;
  int a;

  for (b = 0; b < 32; b += 8) {
    for (x = 0; x < 128; x++, c++) {
      int value = 0xFF;
      y = b;
      for (a = 0; a < 8; a++, y++) {
        if (board[x][y] == 1) {
          value = value - (0x1 << a);
        }
      }
      screen[c] = value;
    }
  }
  display_screen(0, screen);
}

// Clears the entire screen
void clear_screen() {
  int x;
  int y;
  for (x = 0; x < 128; x++) {
    for (y = 0; y < 32; y++) {
      board[x][y] = 0;
    }
  }
}