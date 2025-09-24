#include "shared/os_state.h"

volatile struct State os_state = {
    .screen_state = {
        .buffer_offset = 0
    },
	.keyboard_state = {
		.scanCodes = {
			.headRead = 0,
			.headWrite = 0
		}
	}
};

multiboot_info *boot_info_arr = (multiboot_info*)0x500;
