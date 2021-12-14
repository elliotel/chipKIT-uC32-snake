/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

extern struct Coordinate{ 
    int x; int y; 
};

extern struct FatCoordinate{
   struct Coordinate a; struct Coordinate b;
};

extern struct Fruit{
   int x1; int x2; int y1; int y2;
};

extern struct Highscore{
   char name[3];
   int score;
   char score_char[3];
};

extern struct AI {
   _Bool enabled;
   struct Fruit target;
   _Bool hard;
};

extern struct Snake{
   struct FatCoordinate body[110];
   _Bool alive;
   int length;
   int directionPointer;
   char previous_direction;
   int movesSinceTurn;
   _Bool skip_remove;
   _Bool turnCCW;
   _Bool turnCW;
   _Bool player_one;
   int score;
   struct AI ai;
};

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork();
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128*2];
/* Declare bitmap array containing board */
extern uint8_t screen[128*4];
/* Declare bits for snek */
extern const uint8_t const snek[32*32];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

extern _Bool board[128][32];

extern _Bool multiplayer;
extern char map;
extern _Bool difficulty;
extern struct Highscore highscores[3];

void initialize_fruits(void);
void initialize_snakes(_Bool);
/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void getinput(struct Snake*);
_Bool update_game();
void enable_interrupt(void);
void update_rotation();
int buttonStatus;
void spawn_fruit();
void update_screen();
void display_update();
//void detect_collition();
void string_to_pixel(int, int, char*, int);
void update_score();
void clear_display();
void clear_score_display();
void display_main_menu();
void set_up_board(char);
void display_end_screen();
