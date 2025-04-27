#ifndef SNAKE_H
#define SNAKE_H

#include "gba.h"




#define GAME_X_MIN 8
#define GAME_X_MAX SCREEN_WIDTH-8
#define GAME_Y_MIN 8
#define GAME_Y_MAX SCREEN_HEIGHT-8

#define GAME_X 1
#define GAME_Y 1
#define GAME_WIDTH 28
#define GAME_HEIGHT 18
#define SNAKE_MAX_SIZE 504 // 18*28

void snake_init();

void snake_tick(u32 t);


#endif
