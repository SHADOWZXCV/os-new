#ifndef OS_STATE_H_
#define OS_STATE_H_
#include "IO/keyboard.h"
#include "memory/memory.h"
#include "types/primitives.h"

extern volatile struct State {
	KeyboardState keyboard_state;
    struct {
        word buffer_offset;
    } screen_state;
} os_state;

typedef struct {
	word memory_low;   // 2 bytes
	word memory_high;  // 2 bytes
	word mmap_addr;    // 2 bytes
	word mmap_offset;  // 2 bytes
	word mmap_length;  // 2 bytes
} multiboot_info;

extern multiboot_info *boot_info_arr;

#endif
