#include "../include/print.h"
#include "../include/vga/vga.h"
#include "../include/IO/io.h"
#include "../include/IO/keyboard.h"
#include "../include/shared/os_state.h"

#ifdef VGA_TEXT_MODE_H_

void print(const char *message) {
	while(*message) {
		putchar(*message++, 0x07);
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset));
}

void printf(const char *message, char hex) {
	while(*message) {
		if (*message == '%' && *(message+1) == 'x') {
			unsigned short cursor = 2; 
			while (cursor) {
				char hexNibble = (hex >> (--cursor) * 4) & 0xF;

				if (hexNibble > 9) {
					putchar('A' + (hexNibble - 10), 0x07);
				} else {
					putchar('0' + hexNibble, 0x07);
				}
			}
			message += 2;
			continue;
		} else if (*message == '%' && *(message+1) == 'b') {
			unsigned short cursor = 8;
			while(cursor) {
				unsigned short isBitSet = (hex >> --cursor) & 0x1;

				putchar(isBitSet ? '1' : '0', 0x07);
			}

			message += 2;
			continue;
		}
		putchar(*message++, 0x07);
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset));
}

char getchar() {
	char c;

	while(!(c = processKeyboardBuffer()));
	// putchar('2', 0x03);

	return c;
}

void putchar(char character, unsigned char color) {
	unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;

	vidmem += os_state.screen_state.buffer_offset * 2;

	if (os_state.screen_state.buffer_offset >= OVERFLOW_VGA_OFFSET) {
		int x;
		unsigned char *vidmem_start = (unsigned char *) BASE_ADDR_VGA;
		vidmem = (unsigned char *) BASE_ADDR_VGA;
		for(x = VGA_MEMORY_WIDTH * 2; x < OVERFLOW_VGA_OFFSET * 2; x++) {
			*vidmem_start++ = *(vidmem + x);
		}
		
		for (x = (OVERFLOW_VGA_OFFSET - VGA_MEMORY_WIDTH) * 2; x < OVERFLOW_VGA_OFFSET * 2; x += 2) {
			*(vidmem + x) = ' ';
			*(vidmem + x + 1) = 0x0F;
		}
		
		
		os_state.screen_state.buffer_offset = (OVERFLOW_VGA_OFFSET / VGA_MEMORY_WIDTH - 1) * VGA_MEMORY_WIDTH;
		vidmem = (unsigned char *) BASE_ADDR_VGA + os_state.screen_state.buffer_offset * 2;
	}

	if(character == '\n') {
		os_state.screen_state.buffer_offset = (os_state.screen_state.buffer_offset / VGA_MEMORY_WIDTH + 1) * VGA_MEMORY_WIDTH;
		vidmem = (unsigned char *) BASE_ADDR_VGA + os_state.screen_state.buffer_offset * 2;
	} else {
		*vidmem = character;
		*(vidmem + 1) = color;
		// critical section
		__asm__("cli");
		os_state.screen_state.buffer_offset++;
		__asm__("sti");
		vidmem += 2;
	}

	// set new cursor position
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset));
}

#endif