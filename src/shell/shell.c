#include "shared/os_state.h"
#include "memory/memory.h"
#include "shell/shell.h"
#include "vga/vga.h"
#include "interrupts/time.h"
#include "print.h"

#ifdef VGA_TEXT_MODE_H_
void print_memory_blocks();
void process(string command) {
    if (strcmp(command, "rsclear")) {
        clrscr();
        print("Xenos OS\nCurrent version: 0.0.1\n");
    } else if (strcmp(command, "clear")) {
        clrscr();
    } else if (strcmp(command, "time")) {
        printf("%d:", hour);
        printf("%d:", minute);
        printf("%d\n", second);
    } else if (strcmp(command, "mem_map")) {
        print_mem_info(true);
    } else if (strcmp(command, "mem_size")) {
        print_mem_info(false);
    } else if (strcmp(command, "mem_blocks")) {
        print_memory_blocks();
    } else {
        print("Unknown command.\n");
    }
}

void print_mem_info(short detailed) {
	printf("size in KB: %d\n", memory.memory_size_kb);
	printf("Left in KB: %d\n", memory.memory_size_kb - manager.used_blocks * 4);

    if (!detailed) {
        return;
    }

    print("========== Memory map ==========\n");
    for(byte i = 0; i < memory.mmap_entries_count; i++) {
        mmap_entry *entry = get_memory_map_entry_details(i);

        printf("Entry %d: ", i);
		printf("Base address: %x - ", entry->base_address);
		printf("Length in KB: %d - ", entry->length / 1024);

        if (entry->type == RESERVED) {
            print("( RESERVED )\n");
        } else {
            print("( AVAILABLE )\n");
        }

		// printf("acpi_null ? %d\n", entry->acpi_null);
    }
}


void reset_input_buffer() {
    while(command_buffer.buffer_size) {
		*(command_buffer.ptr--) = 0;
        command_buffer.buffer_size--;
	}
}

byte add_to_buffer(byte *characters) {
	if (command_buffer.buffer_size == VGA_TEXT_LENGTH) {
		return 1;
	}

    if (!(*characters)) {
        return 1;
    }

    string character_buffer_index = command_buffer.ptr;
    short input_size = 0;

	while(*characters) {
        byte character = *(characters++);
        if (character == '\b') {
            /**
             * @brief 4 scenarios exist here for the shell: ( the functionality here is different from other apps )
             * 1. The character to be removed is on the same line
             * 2. It is on the previous line, and we are past the overflow point
             * 3. It is on the previous line, and we are not past the overflow point
             * 4. There are no characters left to removed:
             * 	a. On the same line
             * 
             */
            if (command_buffer.buffer_size == 0) {
                continue;
            }

            input_size--;
            *(--command_buffer.ptr) = 0;
            command_buffer.buffer_size--;
        } else if (character == '\n') {
            input_size++;
            *(command_buffer.ptr++) = character;
            command_buffer.buffer_size++;
            // TODO: HANDLE NEW LINES SENT, BY RENDERING THEN COMING BACK WITH A FRESH NEW COMMAND
            render_at_cursor(character_buffer_index, input_size);
            *(--command_buffer.ptr) = '\0';
            command_buffer.buffer_size--;

            if (*command_buffer.buffer) {
                process(command_buffer.buffer);
            }
            reset_input_buffer();
            input_size = 0;
            character_buffer_index = command_buffer.ptr;
        } else {
            *(command_buffer.ptr++) = character;
            command_buffer.buffer_size++;
            input_size++;
        }
	}

    if (input_size != 0) {
        // TODO: HANDLE NEW LINES SENT, BY RENDERING THEN COMING BACK WITH A FRESH NEW COMMAND
        render_at_cursor(character_buffer_index, input_size);
    }

	return 0;
}

byte print_raw_text(byte *characters, short size) {
    if (!(*characters)) {
        return 1;
    }

    render_at_cursor(characters, size);

	return 0;
}

byte rem_from_buffer(short length) {
	if(!command_buffer.buffer_size) {
		return 1;
	}

	while(length--) {
		*(command_buffer.ptr--) = 0;
		command_buffer.buffer_size--;
	}

	return 0;
}

void print_memory_blocks() {
    print("Memory blocks status:\n");
    printf("Total blocks: %d\n", manager.blocks_count);
    printf("Used blocks: %d\n", manager.used_blocks);
    printf("Free blocks: %d\n", manager.blocks_count - manager.used_blocks);
    print("\nBlock ranges:\n");

    dword current_block = 0;
    byte current_status = mmanager_is_block_free(0);
    
    for (dword i = 1; i <= manager.blocks_count; i++) {
        byte block_status = mmanager_is_block_free(i);

        // switch up
        if (block_status != current_status || i == manager.blocks_count) {
            printf("Blocks %d", current_block);
            print("-");
            printf("%d: ", i - 1);
            printf("Addr %x", current_block * DEFAULT_BLOCK_SIZE_B);
            print("-");
            printf("%x ", i * DEFAULT_BLOCK_SIZE_B - 1);
            print(current_status ? "FREE\n" : "USED\n");
            
            current_block = i;
            current_status = block_status;
        }
    }
}

#endif