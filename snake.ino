#include <Arduino.h>
#include <SPI.h>

#include <Ucglib.h>  // Required
#include "snake.h"
#include "rewards.h"

#define X1_pin 0
#define Y1_pin 1
#define X2_pin 2
#define Y2_pin 3
#define buzzer_red 6
#define buzzer_blue 5

#define LOW_VALUE 100
#define HIGH_VALUE 900

#define red1 230
#define green1 255
#define blue1 0

#define red2 255
#define green2 0
#define blue2 196

#define MAX_REWARDS 10
// Modify the following two lines to match your hardware
// Also, update calibration parameters below, as necessary
Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);
snake *s1;
snake *s2;

list *rewards = NULL;

int width = 240;
int height = 320;

unsigned long s1_counter = 0;
unsigned long s2_counter = 0;
unsigned long reward_counter = 0;
unsigned long current_time = 0;

bool game_running = true;
int res = 0;

bool snake1_drawed = false;
bool snake2_drawed = false;
bool changed_dir1 = false;
bool changed_dir2 = false;
bool tone_red = false;
bool tone_blue = false;

int stick1_info_x = 500;
int stick1_info_y = 500;
int stick2_info_x = 500;
int stick2_info_y = 500;
int rewards_number = 0;

bool isStickIdle(int stick_info) {
  if (stick_info < HIGH_VALUE && stick_info > LOW_VALUE) {
    return true;
  }

  return false;
}

void drawPixel(int pos_x, int pos_y) {
  ucg.drawPixel(pos_y, pos_x);
}

void printSnake(snake *s) {
  snake_cell *curr_cell = s->snake_head;

  while (curr_cell != NULL) {
    Serial.print(curr_cell->pos_x);
    Serial.print(", ");
    Serial.println(curr_cell->pos_y);
    curr_cell = curr_cell->next;
  }
}

void drawSnake2(snake *s, int r, int g, int b) {
  snake_cell *head = s->snake_head;
  snake_cell *tail = s->snake_tail;

  //if (s->last_x_t != -1 && s->last_y_t != -1) {
  ucg.setColor(0, 0, 0);
  drawPixel(s->last_x_t, s->last_y_t);
  //}
  
  ucg.setColor(r, g, b);
  drawPixel(head->pos_x, head->pos_y);

}

int check_snake_dir1(snake *s, int stick_x, int stick_y) {
  if (stick_x > HIGH_VALUE && isStickIdle(stick_y) && s->direction != LEFT && s->direction != RIGHT) {
    Serial.println("Sarpele se duce in dreapta.");
    s->direction = RIGHT;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, RIGHT);
    return 1;
  } else if (stick_x < LOW_VALUE && isStickIdle(stick_y) && s->direction != LEFT && s->direction != RIGHT) {
    Serial.println("Sarpele se duce in stanga.");
    s->direction = LEFT;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, LEFT);
    return 1;
  } else if (stick_y > HIGH_VALUE && isStickIdle(stick_x) && s->direction != UP && s->direction != DOWN) {
    Serial.println("Sarpele se duce in jos.");
    s->direction = DOWN;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, DOWN);
    return 1;
  } else if (stick_y < LOW_VALUE && isStickIdle(stick_x) && s->direction != UP && s->direction != DOWN) {
    Serial.println("Sarpele se duce in sus.");
    s->direction = UP;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, UP);
    return 1;
  }

  return 0;
}

int check_snake_dir2(snake *s, int stick_x, int stick_y) {
  if (stick_x > HIGH_VALUE && isStickIdle(stick_y) && s->direction != UP && s->direction != DOWN) {
    Serial.println("Sarpele 2 se duce in dreapta.");
    s->direction = UP;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, UP);
    return 1;
  } else if (stick_x < LOW_VALUE && isStickIdle(stick_y) && s->direction != UP && s->direction != DOWN) {
    Serial.println("Sarpele 2 se duce in stanga.");
    s->direction = DOWN;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, DOWN);
    return 1;
  } else if (stick_y > HIGH_VALUE && isStickIdle(stick_x) && s->direction != LEFT && s->direction != RIGHT) {
    Serial.println("Sarpele 2 se duce in jos.");
    s->direction = LEFT;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, LEFT);
    return 1;
  } else if (stick_y < LOW_VALUE && isStickIdle(stick_x) && s->direction != LEFT && s->direction != RIGHT) {
    Serial.println("Sarpele 2 se duce in sus.");
    s->direction = RIGHT;
    add_queue_element(&s->moves, s->snake_head->pos_x, s->snake_head->pos_y, RIGHT);
    return 1;
  }

  return 0;
}

void drawRewards() {
  list *current_reward = rewards;

  while (current_reward != NULL) {
    ucg.setColor(current_reward->r, current_reward->g, current_reward->b);
    drawPixel(current_reward->pos_x - 1, current_reward->pos_y - 1);
    drawPixel(current_reward->pos_x - 1, current_reward->pos_y);
    drawPixel(current_reward->pos_x - 1, current_reward->pos_y + 1);
    
    drawPixel(current_reward->pos_x, current_reward->pos_y - 1);
    drawPixel(current_reward->pos_x, current_reward->pos_y);
    drawPixel(current_reward->pos_x, current_reward->pos_y + 1);

    drawPixel(current_reward->pos_x + 1, current_reward->pos_y - 1);
    drawPixel(current_reward->pos_x + 1, current_reward->pos_y);
    drawPixel(current_reward->pos_x + 1, current_reward->pos_y + 1);

    current_reward = current_reward->next;
  }
}

void undrawReward(int pos_x, int pos_y) {
  ucg.setColor(0, 0, 0);

  drawPixel(pos_x - 1, pos_y - 1);
  drawPixel(pos_x - 1, pos_y);
  drawPixel(pos_x - 1, pos_y + 1);
  
  drawPixel(pos_x, pos_y - 1);
  drawPixel(pos_x, pos_y);
  drawPixel(pos_x, pos_y + 1);

  drawPixel(pos_x + 1, pos_y - 1);
  drawPixel(pos_x + 1, pos_y);
  drawPixel(pos_x + 1, pos_y + 1);
}

void print_score() {
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(0, 20);
  ucg.print("Green: "); ucg.print(s1->snake_size);
  
  ucg.setPrintPos(175, 20);
  ucg.print("Pink: "); ucg.print(s2->snake_size);
}

void check_collision_food(snake *s, int id) {
  int spos_x = s->snake_head->pos_x;
  int spos_y = s->snake_head->pos_y;

  list *current_reward = rewards;
  
  while (current_reward != NULL) {
    if (spos_x >= current_reward->pos_x - 1 && spos_x <= current_reward->pos_x + 1
     && spos_y >= current_reward->pos_y - 1 && spos_y <= current_reward->pos_y + 1) {
      undrawReward(current_reward->pos_x, current_reward->pos_y);
      add_snake_cell(s);
      remove_reward(&rewards, current_reward);
      rewards_number--;
      if (id == 1) {
        tone_red = true;
        tone(buzzer_red, 1000);
      }
      else {
        tone_blue = true;
        tone(buzzer_blue, 1000);
      }
      //print_rewards(rewards);
      print_score();
      break;
    }

    current_reward = current_reward->next;
  }
}

//if snake 1 hits snake 2 => 1, else 0, -1 for draw, 2 for snake 2
int check_collision_snakes(snake *s1, snake *s2) {
  int pos_x = s1->snake_head->pos_x;
  int pos_y = s1->snake_head->pos_y;

  snake_cell *cell = s2->snake_head;
  if (pos_x == cell->pos_x && pos_y == cell->pos_y) {
    return -1;
  }
  cell = cell->next;

  while (cell != NULL) {
    if (pos_x == cell->pos_x && pos_y == cell->pos_y) {
      return 1;
    }
    cell = cell->next;
  }

  pos_x = s2->snake_head->pos_x;
  pos_y = s2->snake_head->pos_y;

  cell = s1->snake_head->next;

  while (cell != NULL) {
    if (pos_x == cell->pos_x && pos_y == cell->pos_y) {
      return 2;
    }
    cell = cell->next;
  }

  return 0;
}

void drawStartingScreen() {
  ucg.clearScreen();
  ucg.setFont(ucg_font_ncenR12_tr);
  ucg.setColor(255, 255, 255);

  ucg.setPrintPos(width / 2 - 35, height / 2);
  ucg.print("SNAKE.io");
  ucg.setPrintPos(width / 2 - 10, height / 2 + 15);
  ucg.print("Cosmin Lovin");

  delay(2000);
  ucg.clearScreen();
}

void setup() {
  Serial.begin(9600);
  pinMode(buzzer_blue, OUTPUT);
  pinMode(buzzer_red, OUTPUT);
  ucg.begin(UCG_FONT_MODE_SOLID);
  
  drawStartingScreen();

  ucg.setColor(255, 255, 255);
  s1 = create_snake(35, 35, RIGHT);
  s2 = create_snake(280, 200, LEFT);
  
  //DRAWING SNAKE1
  ucg.setColor(red1, green1, blue1);
  drawPixel(s1->snake_head->pos_x, s1->snake_head->pos_y);
  for (int i = 0; i < 10; i++) {
    snake_cell* cell = add_snake_cell(s1);
    drawPixel(cell->pos_x, cell->pos_y);
  }

  //DRAWING SNAKE2
  ucg.setColor(red2, green2, blue2);
  drawPixel(s2->snake_head->pos_x, s2->snake_head->pos_y);
  for (int i = 0; i < 10; i++) {
    snake_cell* cell = add_snake_cell(s2);
    drawPixel(cell->pos_x, cell->pos_y);
  }

  print_score();
}

unsigned long tr_counter = 0;
unsigned long tb_counter = 0;
void loop() {
  //print_score();
  if (game_running) {
    current_time = millis();

    check_collision_food(s1, 1);
    check_collision_food(s2, 2);

    if (tone_red) {
      if ((unsigned long)(current_time - tr_counter) >= 100) {
        noTone(buzzer_red);
        tone_red = false;
        tr_counter = current_time;
      }
    } else
      tr_counter = current_time;

    if (tone_blue) {
      if ((unsigned long)(current_time - tb_counter) >= 100) {
        noTone(buzzer_blue);
        tone_blue = false;
        tb_counter = current_time;
      }
    } else
      tb_counter = current_time;

    if ((unsigned long)(current_time - s1_counter) >= 50) {
      snake1_drawed = false;
      move_snake(s1);
      s1_counter = current_time;
    }

    if ((unsigned long)(current_time - s2_counter) >= 50) {
      snake2_drawed = false;
      move_snake(s2);
      s2_counter = current_time;
    }

    if ((unsigned long)(current_time - reward_counter) >= 3000) {
      if (rewards_number < MAX_REWARDS) {
        reward_counter = current_time;
        generate_reward(&rewards);
        rewards_number++;
      }
    }

    if (!snake1_drawed) {
      drawSnake2(s1, red1, green1, blue1);
      snake1_drawed = true;
      drawRewards();
    }

    if (!snake2_drawed) {
      drawSnake2(s2, red2, green2, blue2);
      snake2_drawed = true;
      drawRewards();
    }

    stick1_info_x = analogRead(X1_pin);
    stick1_info_y = analogRead(Y1_pin);

    stick2_info_x = analogRead(X2_pin);
    stick2_info_y = analogRead(Y2_pin);

    res = check_snake_dir1(s1, stick1_info_x, stick1_info_y);
    if (res) {
      move_snake(s1);
      drawSnake2(s1, red1, green1, blue1);
    }
    res = check_snake_dir2(s2, stick2_info_x, stick2_info_y);
    if (res) {
      move_snake(s2);
      drawSnake2(s2, red2, green2, blue2);
    }

    res = check_collision_snakes(s1, s2);
    if (res == -1 || res == 1 || res == 2) {
      game_running = false;
      ucg.clearScreen();
    }
  } else {
    if (res == -1) {
      ucg.setFont(ucg_font_ncenR12_tr);
      ucg.setColor(255, 255, 255);

      ucg.setPrintPos(width / 2 - 25, height / 2);
      ucg.print("Draw!");
    } else if (res == 1) {
      ucg.setFont(ucg_font_ncenR12_tr);
      ucg.setColor(255, 255, 255);

      ucg.setPrintPos(width / 2 - 80, height / 2);
      ucg.print("Pink snake wins!");
    } else if (res == 2) {
      ucg.setFont(ucg_font_ncenR12_tr);
      ucg.setColor(255, 255, 255);

      ucg.setPrintPos(width / 2 - 90, height / 2);
      ucg.print("Green snake wins!");
    }
  }
}