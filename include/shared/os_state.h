#include "../IO/keyboard.h"

#ifndef OS_STATE_H_
#define OS_STATE_H_

extern volatile struct State {
	KeyboardState keyboard_state;
    struct {
        unsigned short buffer_offset;
    } screen_state;
} os_state;

#endif
