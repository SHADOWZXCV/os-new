#include "print.h"
#include "vga/vga.h"
#include "shell/shell.h"
#include "IO/io.h"
#include "IO/keyboard.h"
#include "shared/os_state.h"
#include "shared/math/math.h"
#include "types/primitives.h"

#ifdef VGA_TEXT_MODE_H_

int command_size = 0;

void print(char *message) {
	int size = 0;
	char *temp = message;

	while(*(temp++))
		size++;

	print_raw_text(message, size);
}

void printf(const char *message, qword hex) {
	while(*message) {
		if (*message == '%' && *(message+1) == 'x') {
			word cursor = 8;
			while (cursor) {
				char hexNibble = (hex >> (--cursor) * 4) & 0xF;

				if (hexNibble > 9) {
					putchar('A' + (hexNibble - 10));
				} else {
					putchar('0' + hexNibble);
				}
			}
			message += 2;
			continue;
		} else if (*message == '%' && *(message+1) == 'b') {
			word cursor = 8;
			while(cursor) {
				word isBitSet = (hex >> --cursor) & 0x1;

				putchar(isBitSet ? '1' : '0');
			}

			message += 2;
			continue;
		} else if (*message == '%' && *(message+1) == 'd') {
			unsigned int number;
			if (hex < 0) {
				putchar('-');
				number = (unsigned int)(-(long)hex);
			} else {
				number = (unsigned int) hex;
			}

			int divider = 1;
			char cell = 0;

			// find out the biggest digit on our number
			while (number / divider >= 10) {
				divider *= 10;
			}

			while (divider) {
				cell = number / divider;
				number %= divider;
				divider /= 10;
				putchar('0' + cell);
			}
			message += 2;
			continue;
		}
		putchar(*message++);
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (byte)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (byte)(os_state.screen_state.buffer_offset));
}

char getchar() {
	unsigned char c;
	unsigned char text[2];

	while(!(c = processKeyboardBuffer()));

	text[0] = c;
	text[1] = '\0';
	add_to_buffer(text);
	return c;
}

void putchar(unsigned char c) {
	unsigned char text[2];
	text[0] = c;
	text[1] = '\0';
	// TODO: ADD COLORS
	print_raw_text(text, 2);
}

#endif