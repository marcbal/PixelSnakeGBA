#ifndef BUTTONS_H
#define BUTTONS_H


#include "gba.h"


typedef u8 Button;

#define BUTTON_A      0
#define BUTTON_B      1
#define BUTTON_SELECT 2
#define BUTTON_START  3
#define BUTTON_RIGHT  4
#define BUTTON_LEFT   5
#define BUTTON_UP     6
#define BUTTON_DOWN   7
#define BUTTON_R      8
#define BUTTON_L      9


bool Button_getState(Button key);
bool Button_getUp(Button key);
bool Button_getDown(Button key);

void Button_update(u32 frame);


#endif
