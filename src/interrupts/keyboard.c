#include "../../include/interrupts/keyboard.h"
#include "../../include/IO/io.h"
#include "../../include/shared/os_state.h"

void keyboard_handler() {
	unsigned char scanCode = in(0x60);

	// ACK
	if (scanCode == 0xFA){
		return;
	}

	// reset heads if the buffer empty
	// since there are no other interrupt handlers that can abuse this
	// there is no critical section here
	if (os_state.keyboard_state.scanCodes.isEmpty) {
		os_state.keyboard_state.scanCodes.isEmpty = 0;
		os_state.keyboard_state.scanCodes.headRead = 0;
		os_state.keyboard_state.scanCodes.headWrite = 0;
	}

	os_state.keyboard_state.scanCodes.body[os_state.keyboard_state.scanCodes.headWrite++] = scanCode;
}
