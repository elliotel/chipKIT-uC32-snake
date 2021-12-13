#include <stdint.h>  
#include <pic32mx.h>
#include "mipslab.h" 

int const starting_length = 8;
struct Snake s1;
struct Snake s2;
_Bool fruit_coords[128][32];
char directions[4] = {'l', 'u', 'r', 'd'};
int fruit_num;
struct Fruit fruits[11];

char score_count[3];

void initialize_fruits(){   
    fruit_num = 0;
    int i;
    for (i = 0; i < 11; i++) {
        fruits[i].x1 = 0;
        fruits[i].x2 = 0;
        fruits[i].y1 = 0;
        fruits[i].y2 = 0;
    }
}

void update_score(){
    clear_score_display();
    sprintf(score_count, "%d", s1.score);
    string_to_pixel(1, 8, score_count, 3);
    sprintf(score_count, "%d", s2.score);
    string_to_pixel(1, 24, score_count, 3);
}

void spawn_fruit(){
    //Randomly generates locations for fruit
    if(fruit_num == 10){
        return;
    }
    int x = (rand() % 99) + 27;
    //Ändrade denna till 30, så 0-29
    int y = (rand() % 29) + 1;

    if(board[x][y] != 1
    && board[x+1][y] != 1
    && board[x][y+1] != 1
    && board[x+1][y+1] != 1) {
        /*
        int i, j, k;
        for (i = x - 1; i < x + 3; i++) {
            for (j = y - 1; j < y + 3; j++) {
                for (k = 0; k < fruit_num; k++) {
                    if ((fruits[fruit_num].x1 == i || fruits[fruit_num].x2 == i) && (fruits[fruit_num].y1 == j || fruits[fruit_num].y2 == j)) {
                        return;
                    }
                }
            }
        }
        */
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
    update_screen();
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
}

/*
void detect_collition(){
    int i;
    for(i = 0; i < fruit_num; i++){
        if(fruits[fruit_num].x1 > 0){
            board[2][2] = 1;
        }
        if((s->body[0].a.x == fruits[fruit_num].x1 || s->body[0].a.x == fruits[fruit_num].x2 
        || s->body[0].b.x == fruits[fruit_num].x1 || s->body[0].b.x == fruits[fruit_num].x2) && 
        ( s->body[0].a.y == fruits[fruit_num].y1 || s->body[0].a.y == fruits[fruit_num].y2 
        || s->body[0].b.y == fruits[fruit_num].y1 || s->body[0].b.x == fruits[fruit_num].y2)){
        //remove_fruit();
        board[2][2] = 1;
        score++;    
        break;
        }
    }
}
*/

void visualize(struct Snake* s){
    int i;
    for(i = 0; i < s->length; i++){
        board[s->body[i].a.x][s->body[i].a.y] = 1;
        board[s->body[i].b.x][s->body[i].b.y] = 1;
    }
    update_screen();
}
void update_rotation(){
    getinput(&s1);
    getinput(&s2);
}

void ai_select_target(struct Snake* s) {
    s->ai.target = fruits[rand() % fruit_num];
}

void ai_evaluate_rotation(struct Snake* s) {
    struct Fruit target = s->ai.target;
    if (target.x1 == 0) {
        return;
    }
        if (target.x1 < s->body[0].a.x && target.y1 < s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'u':
                case 'l':
                break;
                case 'r':
                s->turnCCW = 1;
                break;
                case 'd':
                s->turnCW = 1;
                break;
            }
        }
        else if (target.x1 > s->body[0].a.x && target.y1 < s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'u':
                case 'r':
                break;
                case 'l':
                s->turnCW = 1;
                break;
                case 'd':
                s->turnCCW = 1;
                break;
            }
        }
        else if (target.x1 < s->body[0].a.x && target.y1 > s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'l':
                case 'd':
                break;
                case 'u':
                s->turnCCW = 1;
                break;
                case 'r':
                s->turnCW = 1;
                break;
            }
        }
        else if (target.x1 > s->body[0].a.x && target.y1 > s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'r':
                case 'd':
                break;
                case 'u':
                s->turnCW = 1;
                break;
                case 'l':
                s->turnCCW = 1;
                break;
            }
        }
        else if (target.x1 < s->body[0].a.x && target.y1 == s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'l':
                break;
                case 'u':
                case 'r':
                s->turnCCW = 1;
                break;
                case 'd':
                s->turnCW = 1;
                break;
            }
        }
        else if (target.x1 > s->body[0].a.x && target.y1 == s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'r':
                break;
                case 'u':
                case 'l':
                s->turnCW = 1;
                break;
                case 'd':
                s->turnCCW = 1;
                break;
            }
        }
        else if (target.x1 == s->body[0].a.x && target.y1 < s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'u':
                break;
                case 'r':
                case 'd':
                s->turnCCW = 1;
                break;
                case 'l':
                s->turnCW = 1;
                break;
            }
        }
        else if (target.x1 == s->body[0].a.x && target.y1 > s->body[0].a.y) {
            switch(directions[s->directionPointer]) {
                case 'd':
                break;
                case 'l':
                case 'u':
                s->turnCCW = 1;
                break;
                case 'r':
                s->turnCW = 1;
                break;
            }
        }
}

// Evaluates if the user is turning
_Bool evaluate_rotation(struct Snake* s){
    if (s->movesSinceTurn < 2) {
        s->turnCW = 0;
        s->turnCCW = 0;
        return 0;
    }
    s->previous_direction = directions[s->directionPointer];
    
    if (s->ai.enabled) {
        ai_evaluate_rotation(s);
    }

    if (s->turnCW && s->turnCCW) {
        s->turnCW = 0;
        s->turnCCW = 0;
        return 0;
    }
    else if(s->turnCCW){
        s->directionPointer--;
    }
    else if(s->turnCW){
        s->directionPointer++;
    }
    
    // Ensures the pointer loops around if it surpasses the boundaries of the array
    if (s->directionPointer > 3) {
        s->directionPointer = 0;
    }
    else if (s->directionPointer < 0 ) {
        s->directionPointer = 3;
    }

    if (s->turnCW || s->turnCCW) {
        s->movesSinceTurn = 0;
        s->turnCW = 0;
        s->turnCCW = 0;
        return 1;
    }
    else {
        return 0;
    }
}

void calculate_fat_rotation(struct Snake* s) {
    char newDirection = directions[s->directionPointer];

    switch(s->previous_direction){
        case 'u':
            if (newDirection == 'l') {
                s->body[0].a.x = s->body[1].a.x - 1;
                s->body[0].a.y = s->body[1].a.y + 1;
                s->body[0].b.x = s->body[1].b.x - 2;
                s->body[0].b.y = s->body[1].b.y;
                
                s->body[1].a.x = s->body[1].a.x;
                s->body[1].a.y = s->body[1].a.y + 1;
                s->body[1].b.x = s->body[1].b.x - 1;
                s->body[1].b.y = s->body[1].b.y;

                s->body[2].a.x = s->body[2].a.x + 1;
                s->body[2].a.y = s->body[2].a.y;
                s->body[2].b.x = s->body[2].b.x;
                s->body[2].b.y = s->body[2].b.y - 1;
                
            }
            else if (newDirection == 'r') {
                s->body[0].a.x = s->body[1].a.x + 2;
                s->body[0].a.y = s->body[1].a.y;
                s->body[0].b.x = s->body[1].b.x + 1;
                s->body[0].b.y = s->body[1].b.y + 1;

                s->body[1].a.x = s->body[1].a.x + 1;
                s->body[1].a.y = s->body[1].a.y;
                s->body[1].b.x = s->body[1].b.x;
                s->body[1].b.y = s->body[1].b.y + 1;

                s->body[2].a.x = s->body[2].a.x;
                s->body[2].a.y = s->body[2].a.y - 1;
                s->body[2].b.x = s->body[2].b.x - 1;
                s->body[2].b.y = s->body[2].b.y;
            }
        break;
        case 'd':
            if (newDirection == 'l') {
                s->body[0].a.x = s->body[1].a.x - 2;
                s->body[0].a.y = s->body[1].a.y;
                s->body[0].b.x = s->body[1].b.x - 1;
                s->body[0].b.y = s->body[1].b.y - 1;

                s->body[1].a.x = s->body[1].a.x - 1;
                s->body[1].a.y = s->body[1].a.y;
                s->body[1].b.x = s->body[1].b.x;
                s->body[1].b.y = s->body[1].b.y - 1;

                s->body[2].a.x = s->body[2].a.x;
                s->body[2].a.y = s->body[2].a.y + 1;
                s->body[2].b.x = s->body[2].b.x + 1;
                s->body[2].b.y = s->body[2].b.y;
            }
            else if (newDirection == 'r') {
                s->body[0].a.x = s->body[1].a.x + 1;
                s->body[0].a.y = s->body[1].a.y - 1;
                s->body[0].b.x = s->body[1].b.x + 2;
                s->body[0].b.y = s->body[1].b.y;

                s->body[1].a.x = s->body[1].a.x;
                s->body[1].a.y = s->body[1].a.y - 1;
                s->body[1].b.x = s->body[1].b.x + 1;
                s->body[1].b.y = s->body[1].b.y;

                s->body[2].a.x = s->body[2].a.x - 1;
                s->body[2].a.y = s->body[2].a.y;
                s->body[2].b.x = s->body[2].b.x;
                s->body[2].b.y = s->body[2].b.y + 1;
            }
        break;
        case 'l':
            if (newDirection == 'u') {
                s->body[0].a.x = s->body[1].a.x;
                s->body[0].a.y = s->body[1].a.y - 2;
                s->body[0].b.x = s->body[1].b.x + 1;
                s->body[0].b.y = s->body[1].b.y - 1;
                
                s->body[1].a.x = s->body[1].a.x;
                s->body[1].a.y = s->body[1].a.y - 1;
                s->body[1].b.x = s->body[1].b.x + 1;
                s->body[1].b.y = s->body[1].b.y;

                s->body[2].a.x = s->body[2].a.x - 1;
                s->body[2].a.y = s->body[2].a.y;
                s->body[2].b.x = s->body[2].b.x;
                s->body[2].b.y = s->body[2].b.y + 1;
            }
            else if (newDirection == 'd') {
                s->body[0].a.x = s->body[1].a.x + 1;
                s->body[0].a.y = s->body[1].a.y + 1;
                s->body[0].b.x = s->body[1].b.x;
                s->body[0].b.y = s->body[1].b.y + 2;

                s->body[1].a.x = s->body[1].a.x + 1;
                s->body[1].a.y = s->body[1].a.y;
                s->body[1].b.x = s->body[1].b.x;
                s->body[1].b.y = s->body[1].b.y + 1;

                s->body[2].a.x = s->body[2].a.x;
                s->body[2].a.y = s->body[2].a.y - 1;
                s->body[2].b.x = s->body[2].b.x - 1;
                s->body[2].b.y = s->body[2].b.y;
            }
        break;
        case 'r':
            if (newDirection == 'u') {
                s->body[0].a.x = s->body[1].a.x - 1;
                s->body[0].a.y = s->body[1].a.y - 1;
                s->body[0].b.x = s->body[1].b.x;
                s->body[0].b.y = s->body[1].b.y - 2;

                s->body[1].a.x = s->body[1].a.x - 1;
                s->body[1].a.y = s->body[1].a.y;
                s->body[1].b.x = s->body[1].b.x;
                s->body[1].b.y = s->body[1].b.y - 1;

                s->body[2].a.x = s->body[2].a.x;
                s->body[2].a.y = s->body[2].a.y + 1;
                s->body[2].b.x = s->body[2].b.x + 1;
                s->body[2].b.y = s->body[2].b.y;
            }
            else if (newDirection == 'd') {
                s->body[0].a.x = s->body[1].a.x;
                s->body[0].a.y = s->body[1].a.y + 2;
                s->body[0].b.x = s->body[1].b.x - 1;
                s->body[0].b.y = s->body[1].b.y + 1;

                s->body[1].a.x = s->body[1].a.x;
                s->body[1].a.y = s->body[1].a.y + 1;
                s->body[1].b.x = s->body[1].b.x - 1 ;
                s->body[1].b.y = s->body[1].b.y;

                s->body[2].a.x = s->body[2].a.x + 1;
                s->body[2].a.y = s->body[2].a.y;
                s->body[2].b.x = s->body[2].b.x;
                s->body[2].b.y = s->body[2].b.y - 1;
            }
        break;
    }
}

void increase_length(struct Snake* s) {
    s->length++;
    s->skip_remove = 1;
}

_Bool detect_fruit_collision(struct Snake* s) {
    int i;
    _Bool found = 0;
        for(i = 0; i < fruit_num; i++){
        if((s->body[0].a.x == fruits[i].x1 || s->body[0].a.x == fruits[i].x2 
        || s->body[0].b.x == fruits[i].x1 || s->body[0].b.x == fruits[i].x2) && 
        ( s->body[0].a.y == fruits[i].y1 || s->body[0].a.y == fruits[i].y2 
        || s->body[0].b.y == fruits[i].y1 || s->body[0].b.y == fruits[i].y2)) {
            remove_fruit(i);
            s->score++;
            increase_length(s);
            update_score();
            found = 1;
            }
        }
    return found;
}

void detect_collision(struct Snake* s) {
    if (board[s->body[0].a.x][s->body[0].a.y] == 1 || board[s->body[0].b.x][s->body[0].b.y] == 1) {
        
        //Check if point is part of fruit, then length increase and remove fruit
        if (detect_fruit_collision(s)) {
            return;
        }

        s->alive = 0;
    }
}

_Bool move() {
    move_snake(&s1);
    move_snake(&s2);
    if (!(s1.alive && s2.alive)) {
        display_end_screen(&s1, &s2);
        return 1;
    }
    return 0;
}

void move_snake(struct Snake* s){
    if (s->ai.enabled) {
        if (s->ai.target.x1 == 0) {
            if (fruits[0].x1 != 0) {
            ai_select_target(s);
            }
        }
        else {
            int i;
            _Bool target_still_exists = 0;
            for (i = 0; i < fruit_num; i++) {
                if (fruits[i].x1 == s->ai.target.x1 &&
                    fruits[i].x2 == s->ai.target.x2 &&
                    fruits[i].y1 == s->ai.target.y1 &&
                    fruits[i].y2 == s->ai.target.y2) {
                    target_still_exists = 1;
                    break;
                }
            }
            if (!target_still_exists) {
                ai_select_target(s);
            }
        }
    }

    /*
    Temporary solution while there is only 1 snake
    If a second snake is added, replace with something akin to a boolean that skips movement only for the snake(s) that turned instead
    */
    _Bool fat_turn = evaluate_rotation(s);

    int x1 = s->body[0].a.x;
    int x2 = s->body[0].b.x;
    int y1 = s->body[0].a.y;
    int y2 = s->body[0].b.y;

    if (!fat_turn) {

        switch(directions[s->directionPointer]){
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

    if (s->movesSinceTurn < 2) {
        s->movesSinceTurn++;
    }
    
    

    struct FatCoordinate coo1 = s->body[0];
    struct FatCoordinate coo2 = s->body[0];
    int i;
    for(i = 0; i < s->length; i++){
        coo2 = coo1;
        coo1 = s->body[i+1];
        s->body[i+1] = coo2;
    }
    
    if (!fat_turn) {
    s->body[0].a.x = x1;
    s->body[0].b.x = x2;
    s->body[0].a.y = y1;
    s->body[0].b.y = y2;
    }
    else {
        calculate_fat_rotation(s);
    }

    detect_collision(s);

    if (!s->skip_remove) {
    //Resets the tail pixel on the board
    board[s->body[s->length-1].a.x][s->body[s->length-1].a.y] = 0;
    board[s->body[s->length-1].b.x][s->body[s->length-1].b.y] = 0;
    } else {
        s->skip_remove = 0;
    }

    board[s->body[0].a.x][s->body[0].a.y] = 1;
    board[s->body[0].b.x][s->body[0].b.y] = 1;

}

void initialize_snakes(_Bool multiplayer){
    s1.player_one = 1;
    s2.player_one = 0;
    s1.ai.enabled = 0;
    s2.ai.enabled = !multiplayer;
    initialize_snake(&s1);
    initialize_snake(&s2);
}

void initialize_snake(struct Snake* s){
    s->length = starting_length;
    s->movesSinceTurn = 2;
    s->skip_remove = 0;
    s->turnCCW = 0;
    s->turnCW = 0;
    s->score = 0;
    s->alive = 1;

    if (s->ai.enabled) {
        s->ai.target.x1 = 0;
        s->ai.target.y1 = 0;
        s->ai.target.x2 = 0;
        s->ai.target.y2 = 0;
    }

    int i;
    for(i = 0; i < s->length; i++){
        s->body[i].a.x = 0;
        s->body[i].b.x = 0;
        s->body[i].a.y = 0;
        s->body[i].b.y = 0;
    }

    if (s->player_one) {
        s->directionPointer = 2;
        int x = 41;
        int y = 8;
    
        for(i = 0; i < s->length; i++){
            s->body[i].a.x = x - i;
            s->body[i].b.x = x - i;
            s->body[i].a.y = y;
            s->body[i].b.y = y + 1;
        }
    }
    else {
        
        s->directionPointer = 0;
        int x = 113;
        int y = 23;
    
        for(i = 0; i < s->length; i++){
            s->body[i].a.x = x + i;
            s->body[i].b.x = x + i;
            s->body[i].a.y = y;
            s->body[i].b.y = y - 1;
        }
    }
    visualize(s);
}

/*
void increaseLength(struct snake snake*){
    snake.length++;
}

void move(struct snake snake*){
    
}
*/