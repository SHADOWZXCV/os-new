#ifndef VGA_H_
#define VGA_H_
// define the text mode as the default mode
#define VGA_TEXT_MODE_H_

#define CRTC_ADDRESS_REG 0x3D4
#define CRTC_DATA_REG 0x3D5
#define BASE_ADDR_VGA 0xB8000
#define OVERFLOW_VGA_OFFSET 2000
#define VGA_MEMORY_WIDTH 80

typedef struct {
    void (*print)(const char *);
    void (*printf)(const char *, char);
    void (*clrscr)();
} VGA_TEXT;

void create(VGA_TEXT* instance);
void clrscr();

#endif
