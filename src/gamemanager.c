#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

_Bool multiplayer;
char map;
_Bool difficulty;

void clear_menu_arrow(int start) {
    int i, j;
  for(i = 0; i < 14; i++){
    for(j = start; j < 32; j++){
      board[i][j] = 0;
    }
  }
}

void display_snake(int x) {
    int y = 0;
    int tot = 0;
    int i, j;
    for (i = y; i < y + 32; i++) {
        for (j = x; j < x + 32; j++, tot++) {
            board[j][i] = snek[tot];
        }
    }
}

int arrow_point_method(int num_choices){
    int choice = 0;
    int arrow_y;
     _Bool done = 0;
    _Bool back = 0;       
    int arrow_pointer;
    if(num_choices == 2){
        arrow_pointer = 0;
    }
    else{
        arrow_pointer = 2;
    } 
    int button_status = 0;
    while (!done) {
        switch (arrow_pointer) {
            case 0:
                arrow_y = 11;
                break;
            case 1:
                arrow_y = 21;
                break;
            case 2:
                arrow_y = 1;
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
                while (button_status) {
                    button_status = getbtns();
                }
                return 3;
            }
        }

        if (arrow_pointer > (num_choices-1)) {
        arrow_pointer = 0;
        }
        else if (arrow_pointer < 0 ) {
            arrow_pointer = (num_choices-1);
        }

        if(num_choices == 2){
            clear_menu_arrow(11);
        }
        else{
            clear_menu_arrow(0);
        }
        while (button_status) {
            button_status = getbtns();
        }
    }
    return arrow_pointer;
}

void display_main_menu() {

    _Bool selecting = 1;
    char menu = 'p';    
    int result = 0;
    map = 'e';
    _Bool button_status;
    while(selecting){
        
        clear_display();
        display_snake(96);
        result = 0;
        switch(menu){

            //play game or check highscores
            case 'p': 
            string_to_pixel(0, 1, "Welcome to Snake!", 17);
            string_to_pixel(14, 11, "Start game!", 11);
            string_to_pixel(14, 21, "Highscores", 10);
            update_screen();
            result = arrow_point_method(2);
            if (result == 0){
            menu = 'g';
            }
            else if (result == 1) {
            menu = 'h';
            }
            break;

            //singleplayer och multiplayer
            case 'g':
            string_to_pixel(0, 1, "Select mode:", 12);
            string_to_pixel(14, 11, "Singleplayer", 12);
            string_to_pixel(14, 21, "Multiplayer", 11); 
            update_screen();
            result = arrow_point_method(2);
            multiplayer = result;
            if (result == 0){
            menu = 'd';
            }
            else if (result == 1) {
            menu = 'm';
            }
            else if (result == 3) {
            menu = 'p';
            }
            break;

            //Which map?
            case 'm': 
            string_to_pixel(14, 1, "Easy map", 8);
            string_to_pixel(14, 11, "Medium map", 11);
            string_to_pixel(14, 21, "Hard map", 8);
            update_screen();
            result = arrow_point_method(3);
            switch(result){
                case 2:
                    map = 'e';
                    break;
                case 0:
                    map = 'm';
                    break;
                case 1:
                    map = 'h';
                    break;
                case 3:
                    menu = 'g';
                    break;
            }
            if(result != 3){
                selecting = 0;
            }
            break;

            //which difficulty
            case 'd': 
            string_to_pixel(0, 1, "AI difficulty:", 14);
            string_to_pixel(14, 11, "Easy", 4);
            string_to_pixel(14, 21, "Hard", 4);
            update_screen();
            result = arrow_point_method(2);
            if (result == 0){
                difficulty = 0;
                selecting = 0;
            }
            else if (result == 1) {
                difficulty = 1;
                selecting = 0;
            }   
            else if (result == 3) {
                menu = 'g';
            }          
            break;

            //view highscores
            case 'h':
            button_status = 0;
            string_to_pixel(0, 0, "Top 3 highscores:", 17);
            int i,j;
            int y = 8;
            char rank[1];
            char entry[12];
            
            //prints all highscores
            for(i = 0; i < 3; i++, y+=8){
                
                //resets the entry array
                for(j = 0; j < 12; j++){
                    entry[j] = '\0'; 
                }                
                sprintf(rank, "%d",i+1);
                strcat(entry, rank);
                strcat(entry, ": ");
                strcat(entry, highscores[i].name);
                if(highscores[i].score != 0){
                strcat(entry, " - ");
                strcat(entry, highscores[i].score_char);
                }
                string_to_pixel(0, y, entry, 12);       
            }
            update_screen();
            while(!(button_status & 0x1)) {
                button_status = getbtns();
            }
            menu = 'p';
            break;
        }
    }
}

void display_end_screen(struct Snake* s1, struct Snake* s2) {
    clear_display();
    int button_status = 0;
        char player_one_score[3];
        char player_two_score[3];
        sprintf(player_one_score, "%d", s1->score);
        sprintf(player_two_score, "%d", s2->score);
        
    char score_string[22] = "Final score: ";
        strcat(score_string, player_one_score);
        strcat(score_string, " - ");
        strcat(score_string, player_two_score);
    if (!s1->alive && !s2->alive) {
        string_to_pixel(0, 1, "Both players died.", 18);
        
        string_to_pixel(0, 11, score_string, 22);
        if (s1->score > s2->score) {
            string_to_pixel(0, 21, "Player 1 wins.", 14);
        }
        else if (s2->score > s1->score) {
            string_to_pixel(0, 21, "Player 2 wins.", 14);
        }
        else {
            string_to_pixel(0, 21, "It's a draw.", 14);
        }
    }
    else if (!s1->alive) {
        string_to_pixel(0, 1, "Player 1 has died.", 18);
        string_to_pixel(0, 11, score_string, 22);
        string_to_pixel(0, 21, "Player 2 wins.", 14);
    }
    else if (!s2->alive) {
        string_to_pixel(0, 1, "Player 2 has died.", 18);
        string_to_pixel(0, 11, score_string, 22);
        string_to_pixel(0, 21, "Player 1 wins.", 14);
        }
            
    else {
        if (s1->score > s2->score) {
        string_to_pixel(0, 1, "Player 1 reached 100 score!", 31);
        string_to_pixel(0, 11, score_string, 22);
        string_to_pixel(0, 21, "Player 1 wins.", 14);
        }
        else if (s2->score > s1->score) {
        string_to_pixel(0, 1, "Player 2 reached 100 score!", 31);
        string_to_pixel(0, 11, score_string, 22);
        string_to_pixel(0, 21, "Player 2 wins.", 14); 
        }
        else {
        string_to_pixel(0, 1, "Both reached 100 score!", 28);
        string_to_pixel(0, 11, score_string, 22);
        string_to_pixel(0, 21, "It's a draw.", 14);
        }
    }
        update_screen();
        while(!(button_status & 0x1)) {
            button_status = getbtns();
        }
        clear_display();
        if (s2->ai.enabled && (s1->score > highscores[2].score)) {
            int highscore_rank;
            if (s1->score > highscores[1].score) {
                if (s1->score > highscores[0].score) {
                    highscore_rank = 0;
                    highscores[2] = highscores[1];
                    highscores[1] = highscores[0];
                }
                else {
                    highscore_rank = 1;
                    highscores[2] = highscores[1];
                }
            }
            else {
                highscore_rank = 2;
            }

            int i;
            int alphabet_index = 0;
            char name[3] = "___";
            char name_spaced[5];
            for (i = 0; i < 3; i++) {

                char highscore_string[25] = "You got highscore rank ";
                char highscore_rank_string[1];
                sprintf(highscore_rank_string, "%d", highscore_rank + 1);
            
                strcat(highscore_string, highscore_rank_string);
                strcat(highscore_string, "!");

                string_to_pixel(0, 1, highscore_string, 25);
                string_to_pixel(0, 11, "Enter name (3 letters):", 23);

                _Bool done = 0;
                alphabet_index = 0;
                while ( !done ) {
                    name[i] = alphabet[alphabet_index];
                    name_spaced[0] = name[0];
                    name_spaced[1] = name_spaced[3] = ' ';
                    name_spaced[2] = name[1];
                    name_spaced[4] = name[2];

                    string_to_pixel(0, 21, name_spaced, 5);
                    update_screen();

                    while(!button_status) {
                        button_status = getbtns();
                        if (button_status & 0x8){
                            alphabet_index--;
                        }
                         if (button_status & 0x4) {
                            alphabet_index++;
                        }
                        if (button_status & 0x2){
                            done = 1;
                        }
                        if (button_status & 0x1){
                        }
                    }
                    
                  if (alphabet_index > 25) {
                        alphabet_index = 0;
                    }
                    else if (alphabet_index < 0 ) {
                        alphabet_index = 25;
                    }

                    int x, y;
                    for (x = 0; x < 128; x++) {
                        for (y = 21; y < 32; y++) {
                            board[x][y] = 0;
                        }
                    }
                    
                    while (button_status) {
                    button_status = getbtns();
                    }
                }
                
                 
            
        }
        strcpy(highscores[highscore_rank].name, name);
        highscores[highscore_rank].score = s1->score;
        strcpy(highscores[highscore_rank].score_char, player_one_score);
        return;
        }
        
}