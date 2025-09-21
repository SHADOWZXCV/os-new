#include "IO/io.h"
#include "types/primitives.h"

unsigned char in(word _port) {
	unsigned char res;
	__asm__("in %%dx, %%al" :"=a" (res) : "d" (_port));
	
	return res;
}

void out(word _port, byte data) {
	__asm__("out %%al, %%dx" :: "a" (data), "d" (_port));
}
