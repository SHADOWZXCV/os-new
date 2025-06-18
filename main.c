#define CRTC_ADDRESS_REG 0x3D4
#define CRTC_DATA_REG 0x3D5
#define BASE_ADDR_VGA 0xB8000
#define OVERFLOW_VGA_OFFSET 2000
#define VGA_MEMORY_WIDTH 80

const char *tutorial3  = "Hello world!\nI would like to see how this works!";

unsigned char in(unsigned short _port) {
	unsigned char res;
	__asm__("in %%dx, %%al" :"=a" (res) : "d" (_port));
	
	return res;
}

void out(unsigned short _port, unsigned char data) {
	__asm__("out %%al, %%dx" :: "a" (data), "d" (_port));
}

void print(const char *message) {
	unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;
	unsigned short offset;
	unsigned long i;
	
	out(CRTC_ADDRESS_REG, 14);
	offset = in(CRTC_DATA_REG) << 8;
	out(CRTC_ADDRESS_REG, 15);
	offset |= in(CRTC_DATA_REG);
	
	vidmem += offset * 2;
	
	i = 0;
	while(message[i]) {
		if (offset >= OVERFLOW_VGA_OFFSET) {
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
			
			
			offset = (OVERFLOW_VGA_OFFSET / VGA_MEMORY_WIDTH - 1) * VGA_MEMORY_WIDTH;
			vidmem = (unsigned char *) BASE_ADDR_VGA + offset * 2;
		}

		if(message[i] == '\n') {
			offset = (offset / 80 + 1) * 80;
			vidmem = (unsigned char *) BASE_ADDR_VGA + offset * 2;
			i++;
		} else {
			*vidmem = message[i++];
			offset++;
			vidmem += 2;
		}
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(offset));
}



void clrscr() {
	unsigned char *vidmem = (unsigned char *) 0xB8000;
	
	long ctr;
	
	for (ctr = 0; ctr < 80*25; ctr++) {
		*vidmem++ = 0;
		*vidmem++ = 0xF;
	}	

   out(CRTC_ADDRESS_REG, 14);
   out(CRTC_DATA_REG, 0);
   out(CRTC_ADDRESS_REG, 15);
   out(CRTC_DATA_REG, 0);
}

void main() {
	clrscr();
	print(tutorial3);
	for(;;);
}