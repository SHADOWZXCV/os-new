#define CRTC_ADDRESS_REG 0x3D4
#define CRTC_DATA_REG 0x3D5
#define BASE_ADDR_VGA 0xB8000
#define OVERFLOW_VGA_OFFSET 2000
#define VGA_MEMORY_WIDTH 80


#define COMMAND_PORT_KEYBOARD 0x64
#define STATUS_PORT_KEYBOARD 0x64
#define DATA_PORT_KEYBOARD 0x60
#define KEYBOARD_OUTPUT_BUFFER_STATUS_BIT 0
#define KEYBOARD_INPUT_BUFFER_STATUS_BIT 1
#define STATE_LSHIFT_MASK 0x80
#define STATE_LCTRL_MASK 0x40
#define STATE_LALT_MASK 0x20
#define STATE_RALT_MASK 0x08
#define STATE_RCTRL_MASK 0x04
#define STATE_RSHIFT_MASK 0x02
#define STATE_CAPS_MASK 0x01

#define KEYBOARD_CHAR_RELEASED_MASK 0x04
#define KEYBOARD_CHAR_EXTENDED_MASK 0x02
#define KEYBOARD_CHAR_READY_MASK 0x01


typedef unsigned int size;
typedef struct {
	char body[OVERFLOW_VGA_OFFSET];
	unsigned char isEmpty;
	int headRead;
	int headWrite;
} Buffer;

typedef struct {
	Buffer scanCodes;
	// current keyboard state: lshift|lctrl|ralt|0|lalt|rctrl|rshift|caps
	unsigned char state;
	// current character state: 00000|released|extended|ready
	unsigned char charState;
} KeyboardState;

volatile struct {
	KeyboardState keyboard_state;
} os_state = {
	.keyboard_state = {
		.scanCodes = {
			.headRead = 0,
			.headWrite = 0
		}
	}
};

const char *welcome = "Xenos OS\nCurrent version: 0.0.1\n";
unsigned short offset = 0;

const unsigned char keyboard_map_scancode_set2_us_qwerty[128] = {
	[0x0D] = '\t',   // 0x0D - Tab
	[0x0E] = '`',    // 0x0E - ` (back tick)
	[0x15] = 'q',    // 0x15 - q
	[0x16] = '1',    // 0x16 - 1
	[0x1A] = 'z',    // 0x1A - z
	[0x1B] = 's',    // 0x1B - s
	[0x1C] = 'a',    // 0x1C - a
	[0x1D] = 'w',    // 0x1D - w
	[0x1E] = '2',    // 0x1E - 2
	[0x21] = 'c',    // 0x21 - c
	[0x22] = 'x',    // 0x22 - x
	[0x23] = 'd',    // 0x23 - d
	[0x24] = 'e',    // 0x24 - e
	[0x25] = '4',    // 0x25 - 4
	[0x26] = '3',    // 0x26 - 3
	[0x29] = ' ',    // 0x29 - Space
	[0x2A] = 'v',    // 0x2A - v
	[0x2B] = 'f',    // 0x2B - f
	[0x2C] = 't',    // 0x2C - t
	[0x2D] = 'r',    // 0x2D - r
	[0x2E] = '5',    // 0x2E - 5
	[0x31] = 'n',    // 0x31 - n
	[0x32] = 'b',    // 0x32 - b
	[0x33] = 'h',    // 0x33 - h
	[0x34] = 'g',    // 0x34 - g
	[0x35] = 'y',    // 0x35 - y
	[0x36] = '6',    // 0x36 - 6
	[0x3A] = 'm',    // 0x3A - m
	[0x3B] = 'j',    // 0x3B - j
	[0x3C] = 'u',    // 0x3C - u
	[0x3D] = '7',    // 0x3D - 7
	[0x3E] = '8',    // 0x3E - 8
	[0x41] = ',',    // 0x41 - , (comma)
	[0x42] = 'k',    // 0x42 - k
	[0x43] = 'i',    // 0x43 - i
	[0x44] = 'o',    // 0x44 - o
	[0x45] = '0',    // 0x45 - 0 (zero)
	[0x46] = '9',    // 0x46 - 9
	[0x49] = '.',    // 0x49 - . (period)
	[0x4A] = '/',    // 0x4A - / (slash)
	[0x4B] = 'l',    // 0x4B - l
	[0x4C] = ';',    // 0x4C - ; (semicolon)
	[0x4D] = 'p',    // 0x4D - p
	[0x4E] = '-',    // 0x4E - - (minus)
	[0x52] = '\'',   // 0x52 - ' (apostrophe)
	[0x54] = '[',    // 0x54 - [ (left bracket)
	[0x55] = '=',    // 0x55 - = (equals)
	[0x5A] = '\n',   // 0x5A - Enter
	[0x5B] = ']',    // 0x5B - ] (right bracket)
	[0x5D] = '\\',   // 0x5D - \ (backslash)
	[0x66] = '\b',   // 0x66 - Backspace
	[0x69] = '1',    // 0x69 - Keypad 1
	[0x6B] = '4',    // 0x6B - Keypad 4
	[0x6C] = '7',    // 0x6C - Keypad 7
	[0x70] = '0',    // 0x70 - Keypad 0
	[0x71] = '.',    // 0x71 - Keypad . (decimal point)
	[0x72] = '2',    // 0x72 - Keypad 2
	[0x73] = '5',    // 0x73 - Keypad 5
	[0x74] = '6',    // 0x74 - Keypad 6
	[0x75] = '8',    // 0x75 - Keypad 8
	[0x79] = '+',    // 0x79 - Keypad +
	[0x7A] = '3',    // 0x7A - Keypad 3
	[0x7B] = '-',    // 0x7B - Keypad -
	[0x7C] = '*',    // 0x7C - Keypad *
	[0x7D] = '9',    // 0x7D - Keypad 9
};

const unsigned char extended_set_2_us_qwerty[128] = {
	[0x70] = '0',
	[0x71] = '.',
	[0x72] = '2',
	[0x73] = '5',
	[0x74] = '6',
	[0x75] = '8',
	[0x76] = '9',
	[0x79] = '+',
	[0x7A] = '3',
	[0x7B] = '-',
	[0x7C] = '*',
	[0x7D] = '9',
	[0x69] = '1',
	[0x6B] = '4',
	[0x6C] = '7',
	[0x5A] = '\n',
	[0x5B] = ']',
	[0x4E] = '-',
	[0x4C] = ';',
	[0x4A] = '/',
	[0x49] = '.',
	[0x41] = ',',
	[0x3E] = '8',
	[0x3D] = '7',
	[0x3C] = 'u',
	[0x3B] = 'j',
	[0x3A] = 'm',
	[0x36] = '6',
	[0x35] = 'y',
	[0x34] = 'g',
	[0x33] = 'h',
	[0x32] = 'b',
	[0x31] = 'n',
};

unsigned char upper_case_us_qwerty[256] = { 
	[0x0D] = '\t',   // 0x0D - Tab
	[0x0E] = '`',    // 0x0E - ` (back tick)
	[0x15] = 'Q',    // 0x15 - Q
	[0x16] = '1',    // 0x16 - 1
	[0x1A] = 'Z',    // 0x1A - Z
	[0x1B] = 'S',    // 0x1B - S
	[0x1C] = 'A',    // 0x1C - A
	[0x1D] = 'W',    // 0x1D - W
	[0x1E] = '2',    // 0x1E - 2
	[0x21] = 'C',    // 0x21 - C
	[0x22] = 'X',    // 0x22 - X
	[0x23] = 'D',    // 0x23 - D
	[0x24] = 'E',    // 0x24 - E
	[0x25] = '4',    // 0x25 - 4
	[0x26] = '3',    // 0x26 - 3
	[0x29] = ' ',    // 0x29 - Space
	[0x2A] = 'V',    // 0x2A - V
	[0x2B] = 'F',    // 0x2B - F
	[0x2C] = 'T',    // 0x2C - T
	[0x2D] = 'R',    // 0x2D - R
	[0x2E] = '5',    // 0x2E - 5
	[0x31] = 'N',    // 0x31 - N
	[0x32] = 'B',    // 0x32 - B
	[0x33] = 'H',    // 0x33 - H
	[0x34] = 'G',    // 0x34 - G
	[0x35] = 'Y',    // 0x35 - Y
	[0x36] = '6',    // 0x36 - 6
	[0x3A] = 'M',    // 0x3A - M
	[0x3B] = 'J',    // 0x3B - J
	[0x3C] = 'U',    // 0x3C - U
	[0x3D] = '7',    // 0x3D - 7
	[0x3E] = '8',    // 0x3E - 8
	[0x41] = ',',    // 0x41 - , (comma)
	[0x42] = 'K',    // 0x42 - K
	[0x43] = 'I',    // 0x43 - I
	[0x44] = 'O',    // 0x44 - O
	[0x45] = '0',    // 0x45 - 0 (zero)
	[0x46] = '9',    // 0x46 - 9
	[0x49] = '.',    // 0x49 - . (period)
	[0x4A] = '/',    // 0x4A - / (slash)
	[0x4B] = 'L',    // 0x4B - L
	[0x4C] = ';',    // 0x4C - ; (semicolon)
	[0x4D] = 'P',    // 0x4D - P
	[0x4E] = '-',    // 0x4E - - (minus)
	[0x52] = '\'',   // 0x52 - ' (apostrophe)
	[0x54] = '[',    // 0x54 - [ (left bracket)
	[0x55] = '=',    // 0x55 - = (equals)
	[0x5A] = '\n',   // 0x5A - Enter
	[0x5B] = ']',    // 0x5B - ] (right bracket)
	[0x5D] = '\\',   // 0x5D - \ (backslash)
	[0x66] = '\b',   // 0x66 - Backspace
	[0x69] = '1',    // 0x69 - Keypad 1
	[0x6B] = '4',    // 0x6B - Keypad 4
	[0x6C] = '7',    // 0x6C - Keypad 7
	[0x70] = '0',    // 0x70 - Keypad 0
	[0x71] = '.',    // 0x71 - Keypad . (decimal point)
	[0x72] = '2',    // 0x72 - Keypad 2
	[0x73] = '5',    // 0x73 - Keypad 5
	[0x74] = '6',    // 0x74 - Keypad 6
	[0x75] = '8',    // 0x75 - Keypad 8
	[0x79] = '+',    // 0x79 - Keypad +
	[0x7A] = '3',    // 0x7A - Keypad 3
	[0x7B] = '-',    // 0x7B - Keypad -
	[0x7C] = '*',    // 0x7C - Keypad *
	[0x7D] = '9',    // 0x7D - Keypad 9
};

unsigned char shift_characters_us_qwerty[256] = {
	[0x0D] = '\t',   // 0x0D - Tab (TODO: IMPLEMENT: Back Tab)
	[0x0E] = '~',    // 0x0E - ~ (tilde)
	[0x15] = 'Q',    // 0x15 - Q
	[0x16] = '!',    // 0x16 - ! (exclamation mark)
	[0x1A] = 'Z',    // 0x1A - Z
	[0x1B] = 'S',    // 0x1B - S
	[0x1C] = 'A',    // 0x1C - A
	[0x1D] = 'W',    // 0x1D - W
	[0x1E] = '@',    // 0x1E - @ (at sign)
	[0x21] = 'C',    // 0x21 - C
	[0x22] = 'X',    // 0x22 - X
	[0x23] = 'D',    // 0x23 - D
	[0x24] = 'E',    // 0x24 - E
	[0x25] = '$',    // 0x25 - $ (dollar sign)
	[0x26] = '#',    // 0x26 - # (hash/pound)
	[0x29] = ' ',    // 0x29 - Space
	[0x2A] = 'V',    // 0x2A - V
	[0x2B] = 'F',    // 0x2B - F
	[0x2C] = 'T',    // 0x2C - T
	[0x2D] = 'R',    // 0x2D - R
	[0x2E] = '%',    // 0x2E - % (percent)
	[0x31] = 'N',    // 0x31 - N
	[0x32] = 'B',    // 0x32 - B
	[0x33] = 'H',    // 0x33 - H
	[0x34] = 'G',    // 0x34 - G
	[0x35] = 'Y',    // 0x35 - Y
	[0x36] = '^',    // 0x36 - ^ (caret)
	[0x3A] = 'M',    // 0x3A - M
	[0x3B] = 'J',    // 0x3B - J
	[0x3C] = 'U',    // 0x3C - U
	[0x3D] = '&',    // 0x3D - & (ampersand)
	[0x3E] = '*',    // 0x3E - * (asterisk)
	[0x41] = '<',    // 0x41 - < (less than)
	[0x42] = 'K',    // 0x42 - K
	[0x43] = 'I',    // 0x43 - I
	[0x44] = 'O',    // 0x44 - O
	[0x45] = ')',    // 0x45 - ) (right parenthesis)
	[0x46] = '(',    // 0x46 - ( (left parenthesis)
	[0x49] = '>',    // 0x49 - > (greater than)
	[0x4A] = '?',    // 0x4A - ? (question mark)
	[0x4B] = 'L',    // 0x4B - L
	[0x4C] = ':',    // 0x4C - : (colon)
	[0x4D] = 'P',    // 0x4D - P
	[0x4E] = '_',    // 0x4E - _ (underscore)
	[0x52] = '\"',   // 0x52 - " (double quote)
	[0x54] = '{',    // 0x54 - { (left brace)
	[0x55] = '+',    // 0x55 - + (plus)
	[0x5A] = '\n',   // 0x5A - Enter
	[0x5B] = '}',    // 0x5B - } (right brace)
	[0x5D] = '|',    // 0x5D - | (pipe)
	[0x66] = '\b',   // 0x66 - Backspace
	// TODO: IMPLEMENT NUMLOCK
	[0x69] = '1',    // 0x69 - Keypad 1
	[0x6B] = '4',    // 0x6B - Keypad 4
	[0x6C] = '7',    // 0x6C - Keypad 7
	[0x70] = '0',    // 0x70 - Keypad 0
	[0x71] = '.',    // 0x71 - Keypad . (decimal point)
	[0x72] = '2',    // 0x72 - Keypad 2
	[0x73] = '5',    // 0x73 - Keypad 5
	[0x74] = '6',    // 0x74 - Keypad 6
	[0x75] = '8',    // 0x75 - Keypad 8
	[0x79] = '+',    // 0x79 - Keypad +
	[0x7A] = '3',    // 0x7A - Keypad 3
	[0x7B] = '-',    // 0x7B - Keypad -
	[0x7C] = '*',    // 0x7C - Keypad *
	[0x7D] = '9',    // 0x7D - Keypad 9	
};

unsigned char shift_caps_lock_characters_us_qwerty[256] = {
	[0x0D] = '\t',   // 0x0D - Tab (TODO: IMPLEMENT: Back Tab)
	[0x0E] = '~',    // 0x0E - ~ (tilde)
	[0x15] = 'q',    // 0x15 - q
	[0x16] = '!',    // 0x16 - ! (exclamation mark)
	[0x1A] = 'z',    // 0x1A - z
	[0x1B] = 's',    // 0x1B - s
	[0x1C] = 'a',    // 0x1C - a
	[0x1D] = 'w',    // 0x1D - w
	[0x1E] = '@',    // 0x1E - @ (at sign)
	[0x21] = 'c',    // 0x21 - c
	[0x22] = 'x',    // 0x22 - x
	[0x23] = 'd',    // 0x23 - d
	[0x24] = 'e',    // 0x24 - e
	[0x25] = '$',    // 0x25 - $ (dollar sign)
	[0x26] = '#',    // 0x26 - # (hash/pound)
	[0x29] = ' ',    // 0x29 - Space
	[0x2A] = 'v',    // 0x2A - v
	[0x2B] = 'f',    // 0x2B - f
	[0x2C] = 't',    // 0x2C - t
	[0x2D] = 'r',    // 0x2D - r
	[0x2E] = '%',    // 0x2E - % (percent)
	[0x31] = 'n',    // 0x31 - n
	[0x32] = 'b',    // 0x32 - b
	[0x33] = 'h',    // 0x33 - h
	[0x34] = 'g',    // 0x34 - g
	[0x35] = 'y',    // 0x35 - y
	[0x36] = '^',    // 0x36 - ^ (caret)
	[0x3A] = 'm',    // 0x3A - m
	[0x3B] = 'j',    // 0x3B - j
	[0x3C] = 'u',    // 0x3C - u
	[0x3D] = '&',    // 0x3D - & (ampersand)
	[0x3E] = '*',    // 0x3E - * (asterisk)
	[0x41] = '<',    // 0x41 - < (less than)
	[0x42] = 'k',    // 0x42 - k
	[0x43] = 'i',    // 0x43 - i
	[0x44] = 'o',    // 0x44 - o
	[0x45] = ')',    // 0x45 - ) (right parenthesis)
	[0x46] = '(',    // 0x46 - ( (left parenthesis)
	[0x49] = '>',    // 0x49 - > (greater than)
	[0x4A] = '?',    // 0x4A - ? (question mark)
	[0x4B] = 'l',    // 0x4B - l
	[0x4C] = ':',    // 0x4C - : (colon)
	[0x4D] = 'p',    // 0x4D - p
	[0x4E] = '_',    // 0x4E - _ (underscore)
	[0x52] = '\"',   // 0x52 - " (double quote)
	[0x54] = '{',    // 0x54 - { (left brace)
	[0x55] = '+',    // 0x55 - + (plus)
	[0x5A] = '\n',   // 0x5A - Enter
	[0x5B] = '}',    // 0x5B - } (right brace)
	[0x5D] = '|',    // 0x5D - | (pipe)
	[0x66] = '\b',   // 0x66 - Backspace
	// TODO: IMPLEMENT NUMLOCK
	[0x69] = '1',    // 0x69 - Keypad 1
	[0x6B] = '4',    // 0x6B - Keypad 4
	[0x6C] = '7',    // 0x6C - Keypad 7
	[0x70] = '0',    // 0x70 - Keypad 0
	[0x71] = '.',    // 0x71 - Keypad . (decimal point)
	[0x72] = '2',    // 0x72 - Keypad 2
	[0x73] = '5',    // 0x73 - Keypad 5
	[0x74] = '6',    // 0x74 - Keypad 6
	[0x75] = '8',    // 0x75 - Keypad 8
	[0x79] = '+',    // 0x79 - Keypad +
	[0x7A] = '3',    // 0x7A - Keypad 3
	[0x7B] = '-',    // 0x7B - Keypad -
	[0x7C] = '*',    // 0x7C - Keypad *
	[0x7D] = '9',    // 0x7D - Keypad 9	
};

void putchar(char character, unsigned char color);
char getchar();
void *memset(void *ptr, int value, size size_to_cover);

unsigned char in(unsigned short _port) {
	unsigned char res;
	__asm__("in %%dx, %%al" :"=a" (res) : "d" (_port));
	
	return res;
}

void out(unsigned short _port, unsigned char data) {
	__asm__("out %%al, %%dx" :: "a" (data), "d" (_port));
}

void print(const char *message) {
	unsigned long i = 0;
	
// 	out(CRTC_ADDRESS_REG, 14);
//	offset = in(CRTC_DATA_REG) << 8;
//	out(CRTC_ADDRESS_REG, 15);
//	offset |= in(CRTC_DATA_REG);
	
	while(*message) {
		putchar(*message++, 0x07);
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(offset));
}

void print_hex(const char *message, char hex) {
	unsigned long i = 0;
	
	while(*message) {
		if (*message == '%' && *(message+1) == 'x') {
			unsigned short cursor = 2; 
			while (cursor) {
				char hexNibble = (hex >> (--cursor) * 4) & 0xF;

				if (hexNibble > 9) {
					putchar('A' + (hexNibble - 10), 0x07);
				} else {
					putchar('0' + hexNibble, 0x07);
				}
			}
			message += 2;
			continue;
		} else if (*message == '%' && *(message+1) == 'b') {
			unsigned short cursor = 8;
			while(cursor) {
				unsigned short isBitSet = (hex >> --cursor) & 0x1;

				putchar(isBitSet ? '1' : '0', 0x07);
			}

			message += 2;
			continue;
		}
		putchar(*message++, 0x07);
	}
	
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(offset));
}

__attribute__((section(".keyboard_handler")))
void keyboard_handler() {
	unsigned char scanCode = in(0x60);

	// ACK
	if (scanCode == 0xFA){
		return;
	}

	// reset heads if the buffer empty
	// since there are no other interrupt handlers that can abuse this
	// there is no critical section here
	if (os_state.keyboard_state.scanCodes.isEmpty) {
		os_state.keyboard_state.scanCodes.isEmpty = 0;
		os_state.keyboard_state.scanCodes.headRead = 0;
		os_state.keyboard_state.scanCodes.headWrite = 0;
	}

	os_state.keyboard_state.scanCodes.body[os_state.keyboard_state.scanCodes.headWrite++] = scanCode;
	// putchar('1', 0x03);
}

void update_keyboard_states(unsigned char *scanCode) {
	// Clear the ready flag
	os_state.keyboard_state.charState &= ~KEYBOARD_CHAR_READY_MASK;

	if (*scanCode == 0xF0) {
		os_state.keyboard_state.charState |= KEYBOARD_CHAR_RELEASED_MASK;
        return;
    }

	if (*scanCode == 0xE0) {
		os_state.keyboard_state.charState |= KEYBOARD_CHAR_EXTENDED_MASK;
		return;
	}

	if (os_state.keyboard_state.charState & KEYBOARD_CHAR_RELEASED_MASK){
		os_state.keyboard_state.charState &= ~KEYBOARD_CHAR_RELEASED_MASK;
		// Deactivate special keys if are released
		if (*scanCode == 0x12) {
			os_state.keyboard_state.state &= ~STATE_LSHIFT_MASK;
		} else if (*scanCode == 0x59) {
			os_state.keyboard_state.state &= ~STATE_RSHIFT_MASK;
		} else if (*scanCode == 0x14 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
			os_state.keyboard_state.state &= ~STATE_RCTRL_MASK;
		} else if (*scanCode == 0x14) {
			os_state.keyboard_state.state &= ~STATE_LCTRL_MASK;
		} else if (*scanCode == 0x11 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
			os_state.keyboard_state.state &= ~STATE_RALT_MASK;
		} else if (*scanCode == 0x11) {
			os_state.keyboard_state.state &= ~STATE_LALT_MASK;
		}
		return;
	}

	// Activate special characters if pressed
	if (*scanCode == 0x12) {
		os_state.keyboard_state.state |= STATE_LSHIFT_MASK;
	} else if (*scanCode == 0x59) {
		os_state.keyboard_state.state |= STATE_RSHIFT_MASK;
	} else if (*scanCode == 0x14 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
		os_state.keyboard_state.state |= STATE_RCTRL_MASK;
	} else if (*scanCode == 0x14) {
		os_state.keyboard_state.state |= STATE_LCTRL_MASK;
	} else if (*scanCode == 0x11 && os_state.keyboard_state.charState & KEYBOARD_CHAR_EXTENDED_MASK) {
		os_state.keyboard_state.state |= STATE_RALT_MASK;
	} else if (*scanCode == 0x11) {
		os_state.keyboard_state.state |= STATE_LALT_MASK;
	} else if (*scanCode == 0x58) {
		os_state.keyboard_state.state ^= STATE_CAPS_MASK;
	}

	// set it to ready
	os_state.keyboard_state.charState |= KEYBOARD_CHAR_READY_MASK;

	// Not related to the VGA buffer, but I set it to be the same number
	if (os_state.keyboard_state.scanCodes.headRead == OVERFLOW_VGA_OFFSET) {
		__asm__("hlt"); // IMPLEMENT DYNAMIC BUFFER OR SOMETHING IDK
	}
}

void wait_before_send_keyboard() {
	while (in(STATUS_PORT_KEYBOARD) & (1 << KEYBOARD_INPUT_BUFFER_STATUS_BIT));
}

void wait_ack_keyboard() {
	while (in(DATA_PORT_KEYBOARD) != 0xFA);
}


void wait_before_read_keyboard() {
	while (!(in(STATUS_PORT_KEYBOARD) & (1 << KEYBOARD_OUTPUT_BUFFER_STATUS_BIT)));
}

void set_keyboard_leds() {
	wait_before_send_keyboard();
	out(DATA_PORT_KEYBOARD, 0xED);
	wait_ack_keyboard();

	wait_before_send_keyboard();
	out(DATA_PORT_KEYBOARD, 0x00);
}

unsigned char processKeyboardBuffer() {
	if (os_state.keyboard_state.scanCodes.isEmpty) {
		return 0;
	}

	// critical section
	if(os_state.keyboard_state.scanCodes.headRead == os_state.keyboard_state.scanCodes.headWrite) {
		__asm__("cli");
		os_state.keyboard_state.scanCodes.isEmpty = 1;
		__asm__("sti");
		return 0;
	}

	unsigned char scanCode = os_state.keyboard_state.scanCodes.body[os_state.keyboard_state.scanCodes.headRead++];
	update_keyboard_states(&scanCode);

	if (!(os_state.keyboard_state.charState & KEYBOARD_CHAR_READY_MASK)) {
		return 0;
	}

	// get the character to be printed on screen
	char c;

	if (
		os_state.keyboard_state.state & STATE_LSHIFT_MASK
		|| os_state.keyboard_state.state & STATE_RSHIFT_MASK
	) {
        if (os_state.keyboard_state.state & STATE_CAPS_MASK) {
            c = shift_caps_lock_characters_us_qwerty[scanCode];
		} else {
			c = shift_characters_us_qwerty[scanCode];
		}
	} else if (os_state.keyboard_state.state & STATE_CAPS_MASK) {
		c = upper_case_us_qwerty[scanCode];
	} else {
		c = keyboard_map_scancode_set2_us_qwerty[scanCode];
	}

	return c;
}


void clrscr() {
	unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;
	
	long ctr;
	
	for (ctr = 0; ctr < OVERFLOW_VGA_OFFSET; ctr++) {
		*vidmem++ = 0;
		*vidmem++ = 0xF;
	}	

   out(CRTC_ADDRESS_REG, 14);
   out(CRTC_DATA_REG, 0);
   out(CRTC_ADDRESS_REG, 15);
   out(CRTC_DATA_REG, 0);
   
   offset = 0;
}


char getchar() {
	char c;

	while(!(c = processKeyboardBuffer()));
	// putchar('2', 0x03);

	return c;
}


void putchar(char character, unsigned char color) {
	unsigned char *vidmem = (unsigned char *) BASE_ADDR_VGA;

	vidmem += offset * 2;

	if (offset >= OVERFLOW_VGA_OFFSET) {
		int x;
		unsigned char *vidmem_start = (unsigned char *) BASE_ADDR_VGA;
		vidmem = (unsigned char *) BASE_ADDR_VGA;
		for(x = VGA_MEMORY_WIDTH * 2; x < OVERFLOW_VGA_OFFSET * 2; x++) {
			*vidmem_start++ = *(vidmem + x);
		}
		
		for (x = (OVERFLOW_VGA_OFFSET - VGA_MEMORY_WIDTH) * 2; x < OVERFLOW_VGA_OFFSET * 2; x += 2) {
			*(vidmem + x) = ' ';
			*(vidmem + x + 1) = 0x0F;
		}
		
		
		offset = (OVERFLOW_VGA_OFFSET / VGA_MEMORY_WIDTH - 1) * VGA_MEMORY_WIDTH;
		vidmem = (unsigned char *) BASE_ADDR_VGA + offset * 2;
	}

	if(character == '\n') {
		offset = (offset / VGA_MEMORY_WIDTH + 1) * VGA_MEMORY_WIDTH;
		vidmem = (unsigned char *) BASE_ADDR_VGA + offset * 2;
	} else {
		*vidmem = character;
		*(vidmem + 1) = color;
		// critical section
		__asm__("cli");
		offset++;
		__asm__("sti");
		vidmem += 2;
	}

	// set new cursor position
	out(CRTC_ADDRESS_REG, 14);
	out(CRTC_DATA_REG, (unsigned char)(offset >> 8));
	out(CRTC_ADDRESS_REG, 15);
	out(CRTC_DATA_REG, (unsigned char)(offset));
}

void main() {
	set_keyboard_leds();
	clrscr();
	print(welcome);
	
	char c;
	while (1) {
		// shell
		print("OS>");
		while((c = getchar()) != '\n') {
			putchar(c, 0x07);
		}
		putchar('\n', 0x07);
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