#include <buttons.h>

#include <gba_input.h>


u16 currStates;
u16 prevStates;


bool Button_getState(Button key) {
    return currStates & key;
}
bool Button_getUp(Button key) {
    return (~currStates &  prevStates) & key;
}
bool Button_getDown(Button key) {
    return ( currStates & ~prevStates) & key;
}


void Button_update() {
    prevStates = currStates;
    currStates = ~REG_KEYINPUT & 0x3FF;
}
