#include "print.h"
#include "IO/io.h"
#include "shared/os_state.h"
#include "init.h"

void eisr_handler_no_details(unsigned int vector, unsigned int error) {
	if (vector == 0) {
		print("FATAL ERROR: A DIVISION BY ZERO, HALTING...\n");
		__asm__("hlt");
	}

	return;
}

void PIT_handler() {
	// TODO: DEBUG, IMPLEMENT!
	// print("PIT@!!!\n");
}

void main() {
	char *welcome = "Xenos OS\nCurrent version: 0.0.1\n";
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
