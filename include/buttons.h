#ifndef BUTTONS_H
#define BUTTONS_H

#include <gba_input.h>


typedef u16 Button;

bool Button_getState(Button key);
bool Button_getUp(Button key);
bool Button_getDown(Button key);

void Button_update();


#endif
