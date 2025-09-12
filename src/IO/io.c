#include "../../include/IO/io.h"

unsigned char in(unsigned short _port) {
	unsigned char res;
	__asm__("in %%dx, %%al" :"=a" (res) : "d" (_port));
	
	return res;
}

void out(unsigned short _port, unsigned char data) {
	__asm__("out %%al, %%dx" :: "a" (data), "d" (_port));
}
