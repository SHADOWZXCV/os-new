ENTRY(start)

PHDRS {
  text PT_LOAD FLAGS(5);  /* R + X = 0x4 + 0x1 */
  data PT_LOAD FLAGS(6);  /* R + W = 0x4 + 0x2 */
}

SECTIONS {
	. = 0x12000;

    .keyboard_handler : {
        KEEP(*(.keyboard_handler))
    } :text

	. = 0x12100;
    .idtr : {
        KEEP(*(.idtr))
    } :text
    . = 0x12200;
    .init_idt : {
        KEEP(*(.init_idt))
    } :text
    . = 0x12300;
    .initialize_keyboard : {
        KEEP(*(.initialize_keyboard))
    } :text
	. = 0x12500;

    .start : {
        KEEP(*(.start))
    } :text

    .text : {
        *(.text*)
    } :text

    .rodata : {
        *(.rodata*)
    } :text

    .data : {
        *(.data*)
    } :data

    .bss : {
        *(.bss*)
    } :data

    /* Discard sections that are not needed in the final raw binary */
    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.GNU-stack)
        *(.GCC.command.line)
        *(.stab)
        *(.stabstr)
    }
}