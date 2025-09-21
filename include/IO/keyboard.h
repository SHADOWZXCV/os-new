#ifndef KEYBOARD_H_
#define KEYBOARD_H_
#include "types/primitives.h"

#define COMMAND_PORT_KEYBOARD 0x64
#define STATUS_PORT_KEYBOARD 0x64
#define DATA_PORT_KEYBOARD 0x60
#define KEYBOARD_OUTPUT_BUFFER_STATUS_BIT 0
#define KEYBOARD_INPUT_BUFFER_STATUS_BIT 1
#define STATE_LSHIFT_MASK 0x80
#define STATE_LCTRL_MASK 0x40
#define STATE_LALT_MASK 0x20
#define STATE_RALT_MASK 0x08
#define STATE_RCTRL_MASK 0x04
#define STATE_RSHIFT_MASK 0x02
#define STATE_CAPS_MASK 0x01

#define KEYBOARD_CHAR_RELEASED_MASK 0x04
#define KEYBOARD_CHAR_EXTENDED_MASK 0x02
#define KEYBOARD_CHAR_READY_MASK 0x01

typedef struct {
	char body[2000];
	byte isEmpty;
	int headRead;
	int headWrite;
} Buffer;

typedef struct {
	Buffer scanCodes;
	// current keyboard state: lshift|lctrl|ralt|0|lalt|rctrl|rshift|caps
	byte state;
	// current character state: 00000|released|extended|ready
	byte charState;
} KeyboardState;

void update_keyboard_states(byte *scanCode);
unsigned char processKeyboardBuffer();
void set_keyboard_leds();
void wait_before_read_keyboard();
void wait_ack_keyboard();
void wait_before_send_keyboard();

#endif