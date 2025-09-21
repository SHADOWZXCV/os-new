#ifndef SHELL_H_
#define SHELL_H_
#include "types/primitives.h"
#include "types/string.h"
#define VGA_TEXT_LENGTH 256

typedef struct {
	byte buffer[VGA_TEXT_LENGTH];
	byte buffer_size;
    byte *ptr;
} VGA_COMMAND;

static VGA_COMMAND command_buffer = {
	.buffer = {0},
	.buffer_size = 0,
    .ptr = command_buffer.buffer
};

void print_mem_info(short detailed);
void process(string command);
void reset_input_buffer();
byte add_to_buffer(byte *characters);
byte rem_from_buffer(short length);
byte print_raw_text(byte *characters, short size);

#endif