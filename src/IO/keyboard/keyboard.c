#include "../../include/shared/os_state.h"
#include "../../../include/IO/keyboard.h"
#include "../../../include/IO/io.h"
#include "../../../include/IO/keyboard/char_map.h"

void update_keyboard_states(unsigned char *scanCode) {
	// Clear the ready flag
	os_state.keyboard_state.charState &= ~KEYBOARD_CHAR_READY_MASK;

	if (*scanCode == 0xF0) {
		os_state.keyboard_state.charState |= KEYBOARD_CHAR_RELEASED_MASK;
        return;
    }

	if (*scanCode == 0xE0) {
		os_state.keyboard_state.charState |= KEYBOARD_CHAR_EXTENDED_MASK;
		return;
	}

	if (os_state.keyboard_state.charState & KEYBOARD_CHAR_RELEASED_MASK){
		os_state.keyboard_state.charState &= ~KEYBOARD_CHAR_RELEASED_MASK;
		// Deactivate special keys if are released
		if (*scanCode == 0x12) {
			os_state.keyboard_state.state &= ~STATE_LSHIFT_MASK;
		} else if (*scanCode == 0x59) {
			os_state.keyboard_state.state &= ~STATE_RSHIFT_MASK;
		} else if (*scanCode == 0x14 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
			os_state.keyboard_state.state &= ~STATE_RCTRL_MASK;
		} else if (*scanCode == 0x14) {
			os_state.keyboard_state.state &= ~STATE_LCTRL_MASK;
		} else if (*scanCode == 0x11 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
			os_state.keyboard_state.state &= ~STATE_RALT_MASK;
		} else if (*scanCode == 0x11) {
			os_state.keyboard_state.state &= ~STATE_LALT_MASK;
		}
		return;
	}

	// Activate special characters if pressed
	if (*scanCode == 0x12) {
		os_state.keyboard_state.state |= STATE_LSHIFT_MASK;
	} else if (*scanCode == 0x59) {
		os_state.keyboard_state.state |= STATE_RSHIFT_MASK;
	} else if (*scanCode == 0x14 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
		os_state.keyboard_state.state |= STATE_RCTRL_MASK;
	} else if (*scanCode == 0x14) {
		os_state.keyboard_state.state |= STATE_LCTRL_MASK;
	} else if (*scanCode == 0x11 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
		os_state.keyboard_state.state |= STATE_RALT_MASK;
	} else if (*scanCode == 0x11) {
		os_state.keyboard_state.state |= STATE_LALT_MASK;
	} else if (*scanCode == 0x58) {
		os_state.keyboard_state.state ^= STATE_CAPS_MASK;
	}

	// set it to ready
	os_state.keyboard_state.charState |= KEYBOARD_CHAR_READY_MASK;

	// Not related to the VGA buffer, but I set it to be the same number
	if (os_state.keyboard_state.scanCodes.headRead == 2000) {
		__asm__("hlt"); // IMPLEMENT DYNAMIC BUFFER OR SOMETHING IDK
	}
}

void wait_before_send_keyboard() {
	while (in(STATUS_PORT_KEYBOARD) & (1 << KEYBOARD_INPUT_BUFFER_STATUS_BIT));
}

void wait_ack_keyboard() {
	while (in(DATA_PORT_KEYBOARD) != 0xFA);
}


void wait_before_read_keyboard() {
	while (!(in(STATUS_PORT_KEYBOARD) & (1 << KEYBOARD_OUTPUT_BUFFER_STATUS_BIT)));
}

void set_keyboard_leds() {
	wait_before_send_keyboard();
	out(DATA_PORT_KEYBOARD, 0xED);
	wait_ack_keyboard();

	wait_before_send_keyboard();
	out(DATA_PORT_KEYBOARD, 0x00);
}

unsigned char processKeyboardBuffer() {
	if (os_state.keyboard_state.scanCodes.isEmpty) {
		return 0;
	}

	// critical section
	if(os_state.keyboard_state.scanCodes.headRead == os_state.keyboard_state.scanCodes.headWrite) {
		__asm__("cli");
		os_state.keyboard_state.scanCodes.isEmpty = 1;
		__asm__("sti");
		return 0;
	}

	unsigned char scanCode = os_state.keyboard_state.scanCodes.body[os_state.keyboard_state.scanCodes.headRead++];
	update_keyboard_states(&scanCode);

	if (!(os_state.keyboard_state.charState & KEYBOARD_CHAR_READY_MASK)) {
		return 0;
	}

	// get the character to be printed on screen
	char c;

	if (
		os_state.keyboard_state.state & STATE_LSHIFT_MASK
		|| os_state.keyboard_state.state & STATE_RSHIFT_MASK
	) {
        if (os_state.keyboard_state.state & STATE_CAPS_MASK) {
            c = shift_caps_lock_characters_us_qwerty[scanCode];
		} else {
			c = shift_characters_us_qwerty[scanCode];
		}
	} else if (os_state.keyboard_state.state & STATE_CAPS_MASK) {
		c = upper_case_us_qwerty[scanCode];
	} else {
		c = keyboard_map_scancode_set2_us_qwerty[scanCode];
	}

	return c;
}
