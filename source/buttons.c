


#include "buttons.h"



#define BUTTON_COUNT 10


bool currStates[BUTTON_COUNT];
bool prevStates[BUTTON_COUNT];




bool Button_getState(Button key) {
    return currStates[key];
}
bool Button_getUp(Button key) {
    return prevStates[key] && !currStates[key];
}
bool Button_getDown(Button key) {
    return !prevStates[key] && currStates[key];
}


void Button_update(u32 frame) {
    u16 input = ~KEYS;
    for (u8 i = 0; i < BUTTON_COUNT; i++) {
        prevStates[i] = currStates[i];
        currStates[i] = (input & (1 << i)) ? true : false;
    }
}
