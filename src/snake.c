/* snake.c
   This file written 2021-12-14 by Elliot Elmenbeck and Isak Karlander */

#include <pic32mx.h>
#include <stdint.h>
#include "snake.h"

int const starting_length = 8;
struct Snake s1;
struct Snake s2;
_Bool fruit_coords[128][32];
char directions[4] = {'l', 'u', 'r', 'd'};
int fruit_num;
struct Fruit fruits[11];

char score_count[3];

// Initializes the fruits in fruits[]
void initialize_fruits() {
  fruit_num = 0;
  int i;
  for (i = 0; i < 11; i++) {
    fruits[i].x1 = 0;
    fruits[i].x2 = 0;
    fruits[i].y1 = 0;
    fruits[i].y2 = 0;
  }
}

// Updates the scoreboard
void update_score() {
  clear_score_display();
  sprintf(score_count, "%d", s1.score);
  string_to_pixel(14, 8, score_count, 3);
  sprintf(score_count, "%d", s2.score);
  string_to_pixel(14, 24, score_count, 3);
}

// Randomly generates fruits on the board
void spawn_fruit() {

  if (fruit_num == 10) {
    return;
  }

  int x = (rand() % 99) + 27;
  int y = (rand() % 29) + 1;

  // Only adds a fruit if the area is clear
  if (board[x][y] != 1 && board[x + 1][y] != 1 && board[x][y + 1] != 1 &&
      board[x + 1][y + 1] != 1) {
    struct Fruit fruit = {.x1 = x, .x2 = x + 1, .y1 = y, .y2 = y + 1};
    fruits[fruit_num] = fruit;

    board[fruit.x1][fruit.y1] = 1;
    board[fruit.x2][fruit.y1] = 1;
    board[fruit.x1][fruit.y2] = 1;
    board[fruit.x2][fruit.y2] = 1;

    fruit_num++;
  }
}

// Removes a fruit from the board
void remove_fruit(int num) {

  board[fruits[num].x1][fruits[num].y1] = 0;
  board[fruits[num].x1][fruits[num].y2] = 0;
  board[fruits[num].x2][fruits[num].y1] = 0;
  board[fruits[num].x2][fruits[num].y2] = 0;

  int i;
  for (i = num; i < fruit_num; i++) {
    fruits[i] = fruits[i + 1];
  }

  fruit_num--;
}

// Updates the rotation of both snakes
void update_rotation() {
  get_input(&s1);
  get_input(&s2);
}

//Selects a new fruit to target for the AI depending on its difficulty
void ai_select_target(struct Snake *s) {
  if (!s->ai.hard) {
    if (fruit_num > 0) {
    s->ai.target = fruits[rand() % fruit_num];
    }
  } else {
    struct Fruit closest_fruit;
    int closest_distance = (abs(s->body[0].a.x - fruits[0].x1)) +
                           (abs(s->body[0].a.y - fruits[0].y1));
    int i;
    for (i = 0; i < fruit_num; i++) {
      int new_distance = (abs(s->body[0].a.x - fruits[i].x1)) +
                         (abs(s->body[0].a.y - fruits[i].y1));
      if (i == 0 || new_distance < closest_distance) {
        closest_fruit = fruits[i];
        closest_distance = new_distance;
      }
    }
    s->ai.target = closest_fruit;
  }
}

//Calculations for how the AI should turn to reach its target
void ai_evaluate_rotation(struct Snake *s) {
  struct Fruit target = s->ai.target;
  if (target.x1 == 0) {
    return;
  }
  if (target.x1 < s->body[0].a.x && target.y1 < s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'u':
    case 'l':
      break;
    case 'r':
      s->turn_CCW = 1;
      break;
    case 'd':
      s->turn_CW = 1;
      break;
    }
  } else if (target.x1 > s->body[0].a.x && target.y1 < s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'u':
    case 'r':
      break;
    case 'l':
      s->turn_CW = 1;
      break;
    case 'd':
      s->turn_CCW = 1;
      break;
    }
  } else if (target.x1 < s->body[0].a.x && target.y1 > s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'l':
    case 'd':
      break;
    case 'u':
      s->turn_CCW = 1;
      break;
    case 'r':
      s->turn_CW = 1;
      break;
    }
  } else if (target.x1 > s->body[0].a.x && target.y1 > s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'r':
    case 'd':
      break;
    case 'u':
      s->turn_CW = 1;
      break;
    case 'l':
      s->turn_CCW = 1;
      break;
    }
  } else if (target.x1 < s->body[0].a.x && target.y1 == s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'l':
      break;
    case 'u':
    case 'r':
      s->turn_CCW = 1;
      break;
    case 'd':
      s->turn_CW = 1;
      break;
    }
  } else if (target.x1 > s->body[0].a.x && target.y1 == s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'r':
      break;
    case 'u':
    case 'l':
      s->turn_CW = 1;
      break;
    case 'd':
      s->turn_CCW = 1;
      break;
    }
  } else if (target.x1 == s->body[0].a.x && target.y1 < s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'u':
      break;
    case 'r':
    case 'd':
      s->turn_CCW = 1;
      break;
    case 'l':
      s->turn_CW = 1;
      break;
    }
  } else if (target.x1 == s->body[0].a.x && target.y1 > s->body[0].a.y) {
    switch (directions[s->direction_pointer]) {
    case 'd':
      break;
    case 'l':
    case 'u':
      s->turn_CCW = 1;
      break;
    case 'r':
      s->turn_CW = 1;
      break;
    }
  }
}

// Evaluates if the user is turning
_Bool evaluate_rotation(struct Snake *s) {
  if (s->moves_since_turn < 2) {
    s->turn_CW = 0;
    s->turn_CCW = 0;
    return 0;
  }
  s->previous_direction = directions[s->direction_pointer];

  if (s->ai.enabled) {
    ai_evaluate_rotation(s);
  }

  if (s->turn_CW && s->turn_CCW) {
    s->turn_CW = 0;
    s->turn_CCW = 0;
    return 0;
  } else if (s->turn_CCW) {
    s->direction_pointer--;
  } else if (s->turn_CW) {
    s->direction_pointer++;
  }

  // Ensures the pointer loops around if it surpasses the boundaries of the
  // array
  if (s->direction_pointer > 3) {
    s->direction_pointer = 0;
  } else if (s->direction_pointer < 0) {
    s->direction_pointer = 3;
  }

  if (s->turn_CW || s->turn_CCW) {
    s->moves_since_turn = 0;
    s->turn_CW = 0;
    s->turn_CCW = 0;
    return 1;
  } else {
    return 0;
  }
}

//Calculates how the front pixels of the snake's body array should be set after a turn
void calculate_wide_rotation(struct Snake *s) {
  char newDirection = directions[s->direction_pointer];

  switch (s->previous_direction) {
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

    } else if (newDirection == 'r') {
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
    } else if (newDirection == 'r') {
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
    } else if (newDirection == 'd') {
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
    } else if (newDirection == 'd') {
      s->body[0].a.x = s->body[1].a.x;
      s->body[0].a.y = s->body[1].a.y + 2;
      s->body[0].b.x = s->body[1].b.x - 1;
      s->body[0].b.y = s->body[1].b.y + 1;

      s->body[1].a.x = s->body[1].a.x;
      s->body[1].a.y = s->body[1].a.y + 1;
      s->body[1].b.x = s->body[1].b.x - 1;
      s->body[1].b.y = s->body[1].b.y;

      s->body[2].a.x = s->body[2].a.x + 1;
      s->body[2].a.y = s->body[2].a.y;
      s->body[2].b.x = s->body[2].b.x;
      s->body[2].b.y = s->body[2].b.y - 1;
    }
    break;
  }
}

//Increases the length of a snake
void increase_length(struct Snake *s) {
  s->length++;
  s->skip_remove = 1;
}

//Detects if a snake has collided with a fruit
_Bool detect_fruit_collision(struct Snake *s) {
  int i;
  _Bool found = 0;
  for (i = 0; i < fruit_num; i++) {
    if ((s->body[0].a.x == fruits[i].x1 || s->body[0].a.x == fruits[i].x2 ||
         s->body[0].b.x == fruits[i].x1 || s->body[0].b.x == fruits[i].x2) &&
        (s->body[0].a.y == fruits[i].y1 || s->body[0].a.y == fruits[i].y2 ||
         s->body[0].b.y == fruits[i].y1 || s->body[0].b.y == fruits[i].y2)) {
      remove_fruit(i);
      s->score++;
      increase_length(s);
      update_score();
      found = 1;
    }
  }
  return found;
}

//Detects if a collision has happened
void detect_collision(struct Snake *s) {
  if (board[s->body[0].a.x][s->body[0].a.y] == 1 ||
      board[s->body[0].b.x][s->body[0].b.y] == 1) {

    //Check if collision was made with part of a fruit, if true increase length and remove fruit
    if (detect_fruit_collision(s)) {
      return;
    }

    //If the collision was made with something other than a fruit, sets snake to dead.
    s->alive = 0;
  }
}

//Moves both snakes and checks if the game should end
_Bool move() {
  move_snake(&s1);
  move_snake(&s2);
  if (!(s1.alive && s2.alive) || s1.score >= 100 || s2.score >= 100) {
    display_end_screen(&s1, &s2);
    return 1;
  }
  return 0;
}

//Moves a snake
void move_snake(struct Snake *s) {
  if (s->ai.enabled) {
    if (s->ai.target.x1 == 0 || (s->ai.hard)) {
      if (fruits[0].x1 != 0) {
        ai_select_target(s);
      }
    } else {
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

  //Boolean that skips regular movement calculation for a snake that has turned
  _Bool wide_turn = evaluate_rotation(s);

  int x1 = s->body[0].a.x;
  int x2 = s->body[0].b.x;
  int y1 = s->body[0].a.y;
  int y2 = s->body[0].b.y;

  if (!wide_turn) {

    switch (directions[s->direction_pointer]) {
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

  if (s->moves_since_turn < 2) {
    s->moves_since_turn++;
  }

  //Moves all values in the snake body array backwards, to make room for new ones at the front spot
  struct wide_coordinate coo1 = s->body[0];
  struct wide_coordinate coo2 = s->body[0];
  int i;
  for (i = 0; i < s->length; i++) {
    coo2 = coo1;
    coo1 = s->body[i + 1];
    s->body[i + 1] = coo2;
  }

  //Calculates new positions depending on if the snake is turning or not
  if (!wide_turn) {
    s->body[0].a.x = x1;
    s->body[0].b.x = x2;
    s->body[0].a.y = y1;
    s->body[0].b.y = y2;
  } else {
    calculate_wide_rotation(s);
  }

  detect_collision(s);

  if (!s->skip_remove) {
    // Resets the tail pixel on the board
    board[s->body[s->length - 1].a.x][s->body[s->length - 1].a.y] = 0;
    board[s->body[s->length - 1].b.x][s->body[s->length - 1].b.y] = 0;
  } else {
    s->skip_remove = 0;
  }

  board[s->body[0].a.x][s->body[0].a.y] = 1;
  board[s->body[0].b.x][s->body[0].b.y] = 1;
}

//Initializes both snakes
void initialize_snakes(_Bool multiplayer) {
  s1.player_one = 1;
  s2.player_one = 0;
  s1.ai.enabled = 0;
  s2.ai.enabled = !multiplayer;
  initialize_snake(&s1);
  initialize_snake(&s2);
}

//Initializes a single snake
void initialize_snake(struct Snake *s) {
  s->length = starting_length;
  s->moves_since_turn = 2;
  s->skip_remove = 0;
  s->turn_CCW = 0;
  s->turn_CW = 0;
  s->score = 0;
  s->alive = 1;

  //Initializes variables only relevant if the snake is AI-controlled
  if (s->ai.enabled) {
    s->ai.target.x1 = 0;
    s->ai.target.y1 = 0;
    s->ai.target.x2 = 0;
    s->ai.target.y2 = 0;
    s->ai.hard = difficulty;
  }

  int i;
  for (i = 0; i < s->length; i++) {
    s->body[i].a.x = 0;
    s->body[i].b.x = 0;
    s->body[i].a.y = 0;
    s->body[i].b.y = 0;
  }

  //Initializes player one
  if (s->player_one) {
    s->direction_pointer = 2;
    int x = 41;
    int y = 8;

    for (i = 0; i < s->length; i++) {
      s->body[i].a.x = x - i;
      s->body[i].b.x = x - i;
      s->body[i].a.y = y;
      s->body[i].b.y = y + 1;
    }
  } 
  //Initializes player two
  else {

    s->direction_pointer = 0;
    int x = 113;
    int y = 23;

    for (i = 0; i < s->length; i++) {
      s->body[i].a.x = x + i;
      s->body[i].b.x = x + i;
      s->body[i].a.y = y;
      s->body[i].b.y = y - 1;
    }
  }
  for (i = 0; i < s->length; i++) {
    board[s->body[i].a.x][s->body[i].a.y] = 1;
    board[s->body[i].b.x][s->body[i].b.y] = 1;
  }
}
