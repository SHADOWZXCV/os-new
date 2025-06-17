#define CRTC_ADDRESS_REG 0x3D4
#define CRTC_DATA_REG 0x3D5

const char *tutorial3  = "Hello world!";

unsigned char in(unsigned short _port) {
	unsigned char res;
	__asm__("in %%dx, %%al" :"=a" (res) : "d" (_port));
	
	return res;
}

void out(unsigned short _port, unsigned char data) {
	__asm__("out %%al, %%dx" :: "a" (data), "d" (_port));
}

void print(const char *message) {
	unsigned char *vidmem = (unsigned char *) 0xB8000;
	unsigned short offset;
	unsigned long i;
	
	out(CRTC_ADDRESS_REG, 14);
	offset = in(CRTC_DATA_REG) << 8;
	out(CRTC_ADDRESS_REG, 15);
	offset |= in(CRTC_DATA_REG);
	
	vidmem += offset * 2;
	
	i = 0;
	while(message[i]) {
		*vidmem = message[i++];
		vidmem += 2;
	}
	
	offset += i;
	
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