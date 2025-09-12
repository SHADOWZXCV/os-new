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

void create(VGA_TEXT* instance) {
    instance->print = print;
    instance->printf = printf;
    instance->clrscr = clrscr;
}
