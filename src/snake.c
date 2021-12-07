#include <stdint.h>  
#include <pic32mx.h>
#include <stdlib.h>
#include "mipslab.h" 

int const starting_length = 8;
struct Snake s;
struct Snake s2;
_Bool fruit_coords[128][32];
 _Bool turnCCW = 0;
 _Bool turnCW = 0;
char directions[4] = {'l', 'u', 'r', 'd'};
int directionPointer = 2;
struct Fruit fruits[10];
int fruit_num = 0;

//Kind of redundant but fastest solution to code
_Bool lastTurnClockwise = 0; 

int movesSinceTurn = 10;


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
    int x = (rand() % 125) + 1;
    int y = (rand() % 29) + 1;

    //Börjat implementera lite logik för att skapa structs av frukter, men oklart om det är bäst
    //struct Fruit fruit = {.x1 = x, .x2 = x+1, .y1 = y, .y2 = y+1};
    if(board[x][y] != 1){
    int i, j;
        for(i = x; i <= x+1; i++){
            for(j = y; j <= y+1; j++){
                fruit_coords[i][j] = 1;
                board[i][j] = 1;
            }
        }
    }
}


void remove_fruit(){
}

void render_fruits(){
}


void visualize(){
    int i;
    for(i = 0; i < starting_length; i++){
        board[s.body[i].a.x][s.body[i].a.y] = 1;
        board[s.body[i].b.x][s.body[i].b.y] = 1;
    }
    update_board();
}
void update_rotation(){
    getinput(&turnCCW, &turnCW);
}

// Evaluates if the user is turning
_Bool evaluate_rotation(){
    if (movesSinceTurn < 2) {
        turnCW = 0;
        turnCCW = 0;
        return 0;
    }
    char previousDirection = directions[directionPointer];
    
    if (turnCW && turnCCW) {
        turnCW = 0;
        turnCCW = 0;
        return 0;
    }
    else if(turnCCW){
        lastTurnClockwise = 0;
        directionPointer--;
    }
    else if(turnCW){
        lastTurnClockwise = 1;
        directionPointer++;
    }
    
    // Ensures the pointer loops around if it surpasses the boundaries of the array
    if (directionPointer > 3) {
        directionPointer = 0;
    }
    else if (directionPointer < 0 ) {
        directionPointer = 3;
    }

    if (turnCW || turnCCW) {
        movesSinceTurn = 0;
        turnCW = 0;
        turnCCW = 0;
        calculate_fat_rotation(previousDirection);
        return 1;
    }
    else {
        return 0;
    }
}

void calculate_fat_rotation(char previousDirection) {
    char newDirection = directions[directionPointer];


    /*
    CODE DUPLICATION FROM MOVE
    SORRY WILL FIX
    */
   
    movesSinceTurn++;
    if (movesSinceTurn == starting_length - 1) {
        if (lastTurnClockwise) {
            board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
            board[s.body[starting_length-2].a.x][s.body[starting_length-2].a.y] = 0;
        }
        else {
            board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
            board[s.body[starting_length-2].b.x][s.body[starting_length-2].b.y] = 0;
        }
    }
    else if (movesSinceTurn == starting_length) {
        if (lastTurnClockwise) {
            board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
            board[s.body[starting_length-2].b.x][s.body[starting_length-2].b.y] = 0;
        }
        else {
            board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
            board[s.body[starting_length-2].a.x][s.body[starting_length-2].a.y] = 0;
        }
    }
    else {
        //Resets the tail pixel on the board
        board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
        board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
    }

    struct FatCoordinate coo1 = s.body[0];
    struct FatCoordinate coo2 = s.body[0];
    int i;
    for(i = 0; i < starting_length; i++){
        coo2 = coo1;
        coo1 = s.body[i+1];
        s.body[i+1] = coo2;
    }
    //Code duplication over

    switch(previousDirection){
        case 'u':
            if (newDirection == 'l') {
                s.body[0].a.x = s.body[1].a.x - 1;
                s.body[0].a.y = s.body[1].a.y + 1;
                s.body[0].b.x = s.body[1].b.x - 2;
            }
            else if (newDirection == 'r') {
                s.body[0].a.x = s.body[1].a.x + 2;
                s.body[0].b.x = s.body[1].b.x + 1;
                s.body[0].b.y = s.body[1].b.y + 1;
            }
        break;
        case 'd':
            if (newDirection == 'l') {
                s.body[0].a.x = s.body[1].a.x - 2;
                s.body[0].b.x = s.body[1].b.x - 1;
                s.body[0].b.y = s.body[1].b.y - 1;
            }
            else if (newDirection == 'r') {
                s.body[0].a.x = s.body[1].a.x + 1;
                s.body[0].a.y = s.body[1].a.y - 1;
                s.body[0].b.x = s.body[1].b.x + 2;
            }
        break;
        case 'l':
            if (newDirection == 'u') {
                s.body[0].a.y = s.body[1].a.y - 2;
                s.body[0].b.x = s.body[1].b.x + 1;
                s.body[0].b.y = s.body[1].b.y - 1;
            }
            else if (newDirection == 'd') {
                s.body[0].a.x = s.body[1].a.x + 1;
                s.body[0].a.y = s.body[1].a.y + 1;
                s.body[0].b.y = s.body[1].b.y + 2;
            }
        break;
        case 'r':
            if (newDirection == 'u') {
                s.body[0].a.x = s.body[1].a.x - 1;
                s.body[0].a.y = s.body[1].a.y - 1;
                s.body[0].b.y = s.body[1].b.y - 2;
            }
            else if (newDirection == 'd') {
                s.body[0].a.y = s.body[1].a.y + 2;
                s.body[0].b.x = s.body[1].b.x - 1;
                s.body[0].b.y = s.body[1].b.y + 1;
            }
        break;
    }
    board[s.body[0].a.x][s.body[0].a.y] = 1;
    board[s.body[0].b.x][s.body[0].b.y] = 1;
}

void move(){

    /*
    Temporary solution while there is only 1 snake
    If a second snake is added, replace with something akin to a boolean that skips movement only for the snake(s) that turned instead
    */
    if (evaluate_rotation()) {
        return;
    }

    int x1 = s.body[0].a.x;
    int x2 = s.body[0].b.x;
    int y1 = s.body[0].a.y;
    int y2 = s.body[0].b.y;

    switch(directions[directionPointer]){
        case 'u':
            y1--;
            y2--;
        break;
        case 'd':
            y1++;
            y2++;
        break;
        case 'l':
            x1--;
            x2--;
        break;
        case 'r':
            x1++;
            x2++;
        break;
    }
    
    movesSinceTurn++;
    if (movesSinceTurn == starting_length - 1) {
        if (lastTurnClockwise) {
            board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
            board[s.body[starting_length-2].a.x][s.body[starting_length-2].a.y] = 0;
        }
        else {
            board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
            board[s.body[starting_length-2].b.x][s.body[starting_length-2].b.y] = 0;
        }
    }
    else if (movesSinceTurn == starting_length) {
        if (lastTurnClockwise) {
            board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
            board[s.body[starting_length-2].b.x][s.body[starting_length-2].b.y] = 0;
        }
        else {
            board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
            board[s.body[starting_length-2].a.x][s.body[starting_length-2].a.y] = 0;
        }
    }
    else {
    //Resets the tail pixel on the board
    board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
    board[s.body[starting_length-1].b.x][s.body[starting_length-1].b.y] = 0;
    }

    struct FatCoordinate coo1 = s.body[0];
    struct FatCoordinate coo2 = s.body[0];
    int i;
    for(i = 0; i < starting_length - 1; i++){
        coo2 = coo1;
        coo1 = s.body[i+1];
        s.body[i+1] = coo2;
    }
    
    s.body[0].a.x = x1;
    s.body[0].b.x = x2;
    s.body[0].a.y = y1;
    s.body[0].b.y = y2;

    board[x1][y1] = 1;
    board[x2][y2] = 1;

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
        s.body[i].a.x = 0;
        s.body[i].b.x = 0;
        s.body[i].a.y = 0;
        s.body[i].b.y = 0;
    }
    int x = 32;
    int y = 16;
    
    for(i = 0; i < starting_length; i++){
        s.body[i].a.x = x - i;
        s.body[i].b.x = x - i;
        s.body[i].a.y = y;
        s.body[i].b.y = y + 1;
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