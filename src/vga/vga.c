#include "../../include/print.h"
#include "../../include/IO/io.h"
#include "../../include/vga/vga.h"
#include "../../include/shared/os_state.h"

void clrscr() {
	unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;
		
	for (long ctr = 0; ctr < OVERFLOW_VGA_OFFSET; ctr++) {
		*vidmem++ = 0;
		*vidmem++ = 0xF;
	}	

   out(CRTC_ADDRESS_REG, 14);
   out(CRTC_DATA_REG, 0);
   out(CRTC_ADDRESS_REG, 15);
   out(CRTC_DATA_REG, 0);
   
   os_state.screen_state.buffer_offset = 0;
}

unsigned char *scroll_screen_times(unsigned char *vidmem, short n) {
    if (n * VGA_MEMORY_WIDTH >= OVERFLOW_VGA_OFFSET) {
        // ERROR!
        return vidmem;
    }

    int x;
    unsigned char *vidmem_start = (unsigned char *) BASE_ADDR_VGA;
    vidmem = (unsigned char *) BASE_ADDR_VGA;
    
    for(x = VGA_MEMORY_WIDTH * 2 * n; x < OVERFLOW_VGA_OFFSET * 2; x++) {
        *vidmem_start++ = *(vidmem + x);
    }
    
    for (x = (OVERFLOW_VGA_OFFSET - (VGA_MEMORY_WIDTH * n)) * 2; x < OVERFLOW_VGA_OFFSET * 2; x += 2) {
        *(vidmem + x) = 0;
        *(vidmem + x + 1) = 0x0F;
    }
    
    os_state.screen_state.buffer_offset = OVERFLOW_VGA_OFFSET - VGA_MEMORY_WIDTH;
    //  os_state.screen_state.buffer_offset = ((OVERFLOW_VGA_OFFSET / VGA_MEMORY_WIDTH - (n)) * VGA_MEMORY_WIDTH) - VGA_MEMORY_WIDTH;
    vidmem = (unsigned char *) BASE_ADDR_VGA + os_state.screen_state.buffer_offset * 2;

    // set new cursor position
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset));

    return vidmem;
}

unsigned char *render_at_cursor(byte *input_start_index, short size) {
    unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;

	vidmem += os_state.screen_state.buffer_offset * 2;

    if (size < 0) {
        size *= -1;
        while (size--) {
            // TODO: Implement backwards cleanup
            vidmem--;
            *--vidmem = 0;
            os_state.screen_state.buffer_offset--;
        }
    } else {
        while (*input_start_index) {
            const char character = *(input_start_index++);
            short character_width = 0;

            if (character == '\n') {
                character_width = VGA_MEMORY_WIDTH;
            } else if (character == '\t') {
                // TODO: IMPLEMENT
            }

            if(character == '\n') {
                if ((os_state.screen_state.buffer_offset + character_width) >= OVERFLOW_VGA_OFFSET) {
                    vidmem = scroll_screen_times(vidmem, 1);
                } else {
                    os_state.screen_state.buffer_offset = (os_state.screen_state.buffer_offset / VGA_MEMORY_WIDTH + 1) * VGA_MEMORY_WIDTH;
                    vidmem = (unsigned char *) BASE_ADDR_VGA + os_state.screen_state.buffer_offset * 2;
                }
            } else {
                *vidmem++ = character;
                *vidmem++ = 0xF;
                os_state.screen_state.buffer_offset++;
                if ((os_state.screen_state.buffer_offset) >= OVERFLOW_VGA_OFFSET) {
                    vidmem = scroll_screen_times(vidmem, 1);
                }
            }

        }
    }

	// set new cursor position
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(os_state.screen_state.buffer_offset));

    return vidmem;
}

int len(unsigned char *buffer) {
    int length = 0;
    while (*buffer) {
        length++;
        *buffer++;
    }

    return length;
}

unsigned char render_frame(unsigned char *buffer) {
    clrscr();
    int size = len(buffer);
    render_at_cursor(buffer, size);
}

void create(VGA_TEXT* instance) {
    instance->print = print;
    instance->printf = printf;
    instance->clrscr = clrscr;
}
