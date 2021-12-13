#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

void clear_menu_arrow() {
    int i, j;
  for(i = 0; i < 14; i++){
    for(j = 11; j < 32; j++){
      board[i][j] = 0;
    }
  }
}

//Extrem code duplication i denna för menyerna, måste fixa en extra metod för valen
_Bool display_main_menu() {

    clear_display();
    
    int timeoutcount = 0;
    _Bool done = 0;
    _Bool back = 0;
    int arrow_pointer = 0;
    int arrow_y = 11;
    _Bool multiplayer = 0;
    int button_status = 0;
    
    while ( 1 ) {

    while (!done) {

        clear_display();

    string_to_pixel(0, 1, "Welcome to Snake!", 17);
    string_to_pixel(14, 11, "Start game!", 11);
    string_to_pixel(14, 21, "Highscores", 10);

        switch (arrow_pointer) {
            case 0:
                arrow_y = 11;
                multiplayer = 0;
                break;
            case 1:
                arrow_y = 21;
                multiplayer = 1;
                break;
        }
        
        string_to_pixel(0, arrow_y, "->", 2);
        update_screen();

        while(!button_status) {
            button_status = getbtns();
            if (button_status & 0x8){
                arrow_pointer--;
            }
            if (button_status & 0x4) {
                arrow_pointer++;
            }
            if (button_status & 0x2){
                done = 1;
            }
            if (button_status & 0x1){
                back = 1;
            }
        }

        if (arrow_pointer > 1) {
        arrow_pointer = 0;
        }
        else if (arrow_pointer < 0 ) {
            arrow_pointer = 1;
        }

        clear_menu_arrow();
        
        while (button_status) {
            button_status = getbtns();
        }
        if (done) {
        if (arrow_pointer == 0) {
            //Pass through and leave the method goodbye
        }
        else if (arrow_pointer == 1) {
            clear_display();
            string_to_pixel(0, 0, "Top 3 highscores:", 17);
            string_to_pixel(0, 8, "1: Kajim Val", 12);
            string_to_pixel(0, 16, "2: Daodac Calle", 15);
            string_to_pixel(0, 24, "3: Zuk Ara", 10);
            update_screen();
            while(!(button_status & 0x1)) {
                button_status = getbtns();
            }
            done = 0;
        }

        
        }
    }

    done = 0;

    clear_display();

    string_to_pixel(0, 1, "Select mode:", 12);
    string_to_pixel(14, 11, "Singleplayer", 12);
    string_to_pixel(14, 21, "Multiplayer", 11);

    string_to_pixel(0, arrow_y, "->", 2);

    update_screen();

    while (!done) {

        switch (arrow_pointer) {
            case 0:
                arrow_y = 11;
                multiplayer = 0;
                break;
            case 1:
                arrow_y = 21;
                multiplayer = 1;
                break;
        }
        
        string_to_pixel(0, arrow_y, "->", 2);
        update_screen();

        while(!button_status) {
            button_status = getbtns();
            if (button_status & 0x8){
                arrow_pointer--;
            }
            if (button_status & 0x4) {
                arrow_pointer++;
            }
            if (button_status & 0x2){
                done = 1;
            }
            if (button_status & 0x1){
                back = 1;
            }
        }

        if (arrow_pointer > 1) {
        arrow_pointer = 0;
        }
        else if (arrow_pointer < 0 ) {
            arrow_pointer = 1;
        }

        clear_menu_arrow();
        
        while (button_status) {
            button_status = getbtns();
        }
        
    }
    
    clear_display();
    
    if (!back) {
        return multiplayer;
    }
    back = 0;
    done = 0;
}
}