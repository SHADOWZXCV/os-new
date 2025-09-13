#include "./include/print.h"
#include "./include/IO/io.h"
#include "./include/shared/os_state.h"
#include "./include/init.h"

typedef unsigned int size;
void *memset(void *ptr, int value, size size_to_cover);

char *welcome = "Xenos OS\nCurrent version: 0.0.1\n";

void main() {
	initOs();
	print(welcome);
	
	char c;
	while (1) {
		// shell
		print("OS>");
		while((c = getchar()) != '\n') {
		}
	}
}

void *memset(void *ptr, int value, size size_to_cover) {
	__asm__("cli");
	char *p = (char *) ptr;

	while(size_to_cover--) {
		*p++ = value;
	}
	__asm__("sti");
	return ptr;
}
