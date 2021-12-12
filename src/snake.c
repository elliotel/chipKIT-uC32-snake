#include <stdint.h>  
#include <pic32mx.h>
#include <stdlib.h>
#include "mipslab.h" 

int const starting_length = 8;
struct Snake s;
_Bool fruit_coords[128][32];
_Bool turnCCW = 0;
_Bool turnCW = 0;
char directions[4] = {'l', 'u', 'r', 'd'};
int directionPointer = 2;
int fruit_num = 0;
int score = 20;
struct Fruit fruits[11];

char score_count[3];

//Kind of redundant but fastest solution to code
_Bool lastTurnClockwise = 0; 

char previous_direction;

int movesSinceTurn = -2;
_Bool turned = 0;

int turns[10];
_Bool turnDirection[10];

void queue_init() {
    int i;
    for (i = 0; i < 10; i++) {
        turns[i] = 0;
        turnDirection[i] = 0;
    }
}

void queue_push(int value, _Bool clockwise) {
    int i = 0;
    while (turns[i] != 0) {
        i++;
        if (i == 10) { return; }
    }
    turns[i] = value;
    turnDirection[i] = clockwise;
}

int queue_pop() {
    int popped = turns[0];
    turns[0] = 0;
    turnDirection[0] = 0;
    int i = 0;
    while (turns[i+1] != 0) {
        turns[i] = turns[i+1];
        turnDirection[i] = turnDirection[i+1];
        turns[i+1] = 0;
        turnDirection[i+1] = 0;
        i++;
        if (i == 10) { break; }
    }
    return popped;
}

int queue_total() {
    int sum = 0;
    int i;
    for (i = 0; i < 10; i++) {
        sum += turns[i];
    }
    return sum;
}

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

void update_score(){
    sprintf(score_count, "%d", score);
    string_to_pixel(1, 10, score_count, 3);
}

void spawn_fruit(){
    //Randomly generates locations for fruit
    if(fruit_num == 10){
        return;
    }
    int x = (rand() % 99) + 28;
    int y = (rand() % 29) + 1;

    
    //Börjat implementera lite logik för att skapa structs av frukter, men oklart om det är bäst
    //struct Fruit fruit = {.x1 = x, .x2 = x+1, .y1 = y, .y2 = y+1};
    if(board[x][y] != 1
    && board[x+1][y] != 1
    && board[x][y+1] != 1
    && board[x+1][y+1] != 1) {
        struct Fruit fruit = {.x1 = x, .x2 = x+1, .y1 = y, .y2 = y+1};
        fruits[fruit_num] = fruit;
    
    /*
    //Inte fin kod
    int i, j;
        for(i = x; i <= x+1; i++){
            for(j = y; j <= y+1; j++){
                board[i][j] = 1;
            }
        }
    */
   
        board[fruit.x1][fruit.y1] = 1;
        board[fruit.x2][fruit.y1] = 1;
        board[fruit.x1][fruit.y2] = 1;
        board[fruit.x2][fruit.y2] = 1;

   fruit_num++;
    }
    
}

void visualize_fruit(){
    int i;
    /*
    for(i = 0; i < fruit_num; i++){
        board[fruits[fruit_num].x1][fruits[fruit_num].y1] = 1;
        board[fruits[fruit_num].x1][fruits[fruit_num].y2] = 1;
        board[fruits[fruit_num].x2][fruits[fruit_num].y1] = 1;
        board[fruits[fruit_num].x2][fruits[fruit_num].y2] = 1;
    }
    */
    update_board();
}


void remove_fruit(int num){
    board[fruits[num].x1][fruits[num].y1] = 0;
    board[fruits[num].x1][fruits[num].y2] = 0;
    board[fruits[num].x2][fruits[num].y1] = 0;
    board[fruits[num].x2][fruits[num].y2] = 0;
    int i;
    for (i = num; i < fruit_num; i++) {
    fruits[i] = fruits[i+1];
    }

    fruit_num--;
    score++;
}

/*
void detect_collition(){
    int i;
    for(i = 0; i < fruit_num; i++){
        if(fruits[fruit_num].x1 > 0){
            board[2][2] = 1;
        }
        if((s.body[0].a.x == fruits[fruit_num].x1 || s.body[0].a.x == fruits[fruit_num].x2 
        || s.body[0].b.x == fruits[fruit_num].x1 || s.body[0].b.x == fruits[fruit_num].x2) && 
        ( s.body[0].a.y == fruits[fruit_num].y1 || s.body[0].a.y == fruits[fruit_num].y2 
        || s.body[0].b.y == fruits[fruit_num].y1 || s.body[0].b.x == fruits[fruit_num].y2)){
        //remove_fruit();
        board[2][2] = 1;
        score++;    
        break;
        }
    }
}
*/

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
    if (turned && movesSinceTurn < 2) {
        turnCW = 0;
        turnCCW = 0;
        return 0;
    }
    previous_direction = directions[directionPointer];
    
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
        turned = 1;
        turnCW = 0;
        turnCCW = 0;
        return 1;
    }
    else {
        return 0;
    }
}

void calculate_fat_rotation() {
    char newDirection = directions[directionPointer];

    //Dubbelkolla igen om (och isåfall varför) dessa 2 rader behövs (jag ska alltså)
    board[s.body[starting_length].a.x][s.body[starting_length].a.y] = 0;
    board[s.body[starting_length].b.x][s.body[starting_length].b.y] = 0;
    switch(previous_direction){
        case 'u':
            if (newDirection == 'l') {
                s.body[0].a.x = s.body[1].a.x - 1;
                s.body[0].a.y = s.body[1].a.y + 1;
                s.body[0].b.x = s.body[1].b.x - 2;
                s.body[0].b.y = s.body[1].b.y;
            }
            else if (newDirection == 'r') {
                s.body[0].a.x = s.body[1].a.x + 2;
                s.body[0].a.y = s.body[1].a.y;
                s.body[0].b.x = s.body[1].b.x + 1;
                s.body[0].b.y = s.body[1].b.y + 1;
            }
        break;
        case 'd':
            if (newDirection == 'l') {
                s.body[0].a.x = s.body[1].a.x - 2;
                s.body[0].a.y = s.body[1].a.y;
                s.body[0].b.x = s.body[1].b.x - 1;
                s.body[0].b.y = s.body[1].b.y - 1;
            }
            else if (newDirection == 'r') {
                s.body[0].a.x = s.body[1].a.x + 1;
                s.body[0].a.y = s.body[1].a.y - 1;
                s.body[0].b.x = s.body[1].b.x + 2;
                s.body[0].b.y = s.body[1].b.y;
            }
        break;
        case 'l':
            if (newDirection == 'u') {
                s.body[0].a.x = s.body[1].a.x;
                s.body[0].a.y = s.body[1].a.y - 2;
                s.body[0].b.x = s.body[1].b.x + 1;
                s.body[0].b.y = s.body[1].b.y - 1;
            }
            else if (newDirection == 'd') {
                s.body[0].a.x = s.body[1].a.x + 1;
                s.body[0].a.y = s.body[1].a.y + 1;
                s.body[0].b.x = s.body[1].b.x;
                s.body[0].b.y = s.body[1].b.y + 2;
            }
        break;
        case 'r':
            if (newDirection == 'u') {
                s.body[0].a.x = s.body[1].a.x - 1;
                s.body[0].a.y = s.body[1].a.y - 1;
                s.body[0].b.x = s.body[1].b.x;
                s.body[0].b.y = s.body[1].b.y - 2;
            }
            else if (newDirection == 'd') {
                s.body[0].a.x = s.body[1].a.x;
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

    detect_collision();

    /*
    Temporary solution while there is only 1 snake
    If a second snake is added, replace with something akin to a boolean that skips movement only for the snake(s) that turned instead
    */
    _Bool fat_turn = evaluate_rotation();

    int x1 = s.body[0].a.x;
    int x2 = s.body[0].b.x;
    int y1 = s.body[0].a.y;
    int y2 = s.body[0].b.y;

    if (!fat_turn) {

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
    }
    
    if (turned) {
    movesSinceTurn++;
    }
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
            board[s.body[starting_length].b.x][s.body[starting_length].b.y] = 0;
        }
        else {
            board[s.body[starting_length-1].a.x][s.body[starting_length-1].a.y] = 0;
            board[s.body[starting_length].a.x][s.body[starting_length].a.y] = 0;
        }
        turned = 0;
        movesSinceTurn = -2;
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
    
    if (!fat_turn) {
    s.body[0].a.x = x1;
    s.body[0].b.x = x2;
    s.body[0].a.y = y1;
    s.body[0].b.y = y2;

    board[x1][y1] = 1;
    board[x2][y2] = 1;
    }
    else { 
        calculate_fat_rotation();
    }

}

void detect_collision() {
    struct FatCoordinate next_coordinate = s.body[0];
    switch(directions[directionPointer]){
        case 'u':
        next_coordinate.a.y = s.body[0].a.y - 1;
        next_coordinate.b.y = s.body[0].b.y - 1;
        break;
        case 'd':
        next_coordinate.a.y = s.body[0].a.y + 1;
        next_coordinate.b.y = s.body[0].b.y + 1;
        break;
        case 'l':
        next_coordinate.a.x = s.body[0].a.x - 1;
        next_coordinate.b.x = s.body[0].b.x - 1;
        break;
        case 'r':
        next_coordinate.a.x = s.body[0].a.x + 1;
        next_coordinate.b.x = s.body[0].b.x + 1;
        break;
    }
    if (board[next_coordinate.a.x][next_coordinate.a.y] == 1 || board[next_coordinate.b.x][next_coordinate.b.y] == 1) {
        //Check if point is part of fruit, then length increase and remove fruit
        //Remember to handle edge case where end of snake just finished turning, make sure it grows in the direction the snake turned from, not straight (or it might grow into a wall if it turned in a corner for example)
        //Remove fruit handled, not length increase so far
        if (detect_fruit_collision(next_coordinate) == 1) {
            return;
        }
            

        //Otherwise, game over.
        int q;
        for (q = 0; q < 128; q++) {
            int r;
            for (r = 0; r < 32; r++) {
                board[q][r] = 1;
            }
        }
    }
}

//Jag får "conflicting type" om jag örsöker returna _Bool och jag FÖRSTÅR inte varför, det funkar på evaluate_rotation() 
int detect_fruit_collision(struct FatCoordinate next_coordinate) {
    int i;
        for(i = 0; i < fruit_num; i++){
        if((next_coordinate.a.x == fruits[i].x1 || next_coordinate.a.x == fruits[i].x2 
        || next_coordinate.b.x == fruits[i].x1 || next_coordinate.b.x == fruits[i].x2) && 
        ( next_coordinate.a.y == fruits[i].y1 || next_coordinate.a.y == fruits[i].y2 
        || next_coordinate.b.y == fruits[i].y1 || next_coordinate.b.x == fruits[i].y2)) {
            remove_fruit(i);
            score++;
            return 1;
            }
        }
    return 0;
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