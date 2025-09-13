#include "shell/shell.h"
#include "vga/vga.h"

#ifdef VGA_TEXT_MODE_H_

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

    unsigned char *character_buffer_index = command_buffer.ptr;
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

#endif