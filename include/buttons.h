#ifndef BUTTONS_H
#define BUTTONS_H

#include <gba_input.h>

/// @brief Basic handling of GBA input.
class Buttons {
    private:
        static u16 currStates;
        static u16 prevStates;

    public:

        /// @brief Update the key status from the actual input register.
        /// To call at the beginning of each frame.
        static inline void update() {
            prevStates = currStates;
            currStates = ~REG_KEYINPUT & 0x3FF;
        }

        /// @brief Tells if the provided key is currently held down.
        /// @param key the keys. Any value from KEYPAD_BITS::KEY_* (gba_input.h).
        /// @return true if the key is held, false otherwise.
        static inline bool isDown(u16 key) {
            return currStates & key;
        }

        /// @brief Tells if the provided key has just been pressed.
        /// @param key the keys. Any value from KEYPAD_BITS::KEY_* (gba_input.h).
        /// @return true if the key is held, false otherwise.
        static inline bool isHit(u16 key) {
            return ( currStates & ~prevStates) & key;
        }
};


#endif
