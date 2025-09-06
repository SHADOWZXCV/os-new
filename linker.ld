ENTRY(start)

PHDRS {
  text PT_LOAD FLAGS(5);  /* R + X = 0x4 + 0x1 */
  data PT_LOAD FLAGS(6);  /* R + W = 0x4 + 0x2 */
}

SECTIONS {
	. = 0x20000;

    .keyboard_handler : {
        KEEP(*(.keyboard_handler))
    } :text
	
	. = 0x20200;

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