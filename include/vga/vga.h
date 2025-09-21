#ifndef VGA_H_
#define VGA_H_
// define the text mode as the default mode
#define VGA_TEXT_MODE_H_
#include "types/primitives.h"
#define CRTC_ADDRESS_REG 0x3D4
#define CRTC_DATA_REG 0x3D5
#define BASE_ADDR_VGA 0xB8000
#define OVERFLOW_VGA_OFFSET 2000
#define VGA_MEMORY_WIDTH 80

typedef struct {
    void (*print)(char *);
    void (*printf)(const char *, qword);
    void (*clrscr)();
} VGA_TEXT;

void create(VGA_TEXT* instance);
byte add_to_buffer(byte *characters);
byte print_raw_text(byte *characters, short size);
unsigned char *render_at_cursor(byte *input_start_index, short size);
void clrscr();

#endif
