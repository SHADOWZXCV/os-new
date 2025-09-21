#include "IO/keyboard.h"
#include "memory/memory.h"

#ifndef OS_STATE_H_
#define OS_STATE_H_

extern volatile struct State {
	KeyboardState keyboard_state;
    struct {
        unsigned short buffer_offset;
    } screen_state;

    // MemoryState
} os_state;

typedef struct {
	unsigned short memory_low;   // 2 bytes
	unsigned short memory_high;  // 2 bytes
	unsigned short mmap_addr;    // 2 bytes
	unsigned short mmap_offset;  // 2 bytes
	unsigned short mmap_length;  // 2 bytes
} multiboot_info;

extern multiboot_info *boot_info_arr;

#endif
