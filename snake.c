#include <stdint.h>  
#include <pic32mx.h>
#include "mipslab.h" 

int const starting_length = 8;
struct Snake s;
struct Snake s2;
_Bool fruit_coords[128][32];
 _Bool turnCCW = 0;
 _Bool turnCW = 0;
char directions[4] = {'l', 'u', 'r', 'd'};
int directionPointer = 2;


void initialize_fruit(){    
	//srand(100);
    int x;
    int y;
    for(x = 0; x < 128; x++){
        for(y = 0; y < 32; y++){
            fruit_coords[x][y] = 0;
        }
    }    
}

void spawn_fruit(){
    //int x = (rand() % 125) + 1;
    //int y = (rand() % 29) + 1;
    
    int x = 50;
    int y = 5;

    if(board[x][y] != 1){
        fruit_coords[x][y] = 1;
        board[x][y] = 1;
        update_board();
    }
}


void visualize(){
    int i;
    for(i = 0; i < starting_length; i++){
        board[s.body[i].x][s.body[i].y] = 1;
    }
    update_board();
}

// Evaluates if the user is turning
void evaluate_rotation(){

    //This should not be called, raise flags instead?
    getinput(&turnCCW, &turnCW);

    if(turnCCW){
        directionPointer--;
        turnCCW = 0;
    }
    if(turnCW){
        directionPointer++;
        turnCW = 0;
    }
    
    // Ensures the pointer loops around if it surpasses the boundaries of the array
    directionPointer = directionPointer % 4;
}

void move(){

    evaluate_rotation();

    int x = s.body[0].x;
    int y = s.body[0].y;

    switch(directions[directionPointer]){
        case 'u':
            y--;
        break;
        case 'd':
            y++;
        break;
        case 'l':
            x--;
        break;
        case 'r':
            x++;
        break;
    }

    //Resets the tail pixel on the board
    board[s.body[starting_length-1].x][s.body[starting_length-1].y] = 0;

    int i;
    for(i = 0; i < starting_length; i++){
        s.body[i+1] = s.body[i];
    }
    s.body[0].x = x;
    s.body[0].y = y;

    board[x][y] = 1;
    update_board();
}


void initialize_snake1(){
    initialize_snake();
}

void initialize_snake2(){
    struct Snake s1;
}

void initialize_snake(){
    s.length = starting_length;

    int i;
    for(i = 0; i < sizeof(s.body); i++){
        s.body[i].x = 0;
        s.body[i].y = 0;
    }
    int x = 32;
    int y = 16;
    
    for(i = 0; i < starting_length; i++){
        s.body[i].x = x - i;
        s.body[i].y = y;
    }
    visualize();
}

/*
void increaseLength(struct snake snake*){
    snake.length++;
}

void move(struct snake snake*){
    
}
*/