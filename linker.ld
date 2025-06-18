ENTRY(main)

PHDRS {
  text PT_LOAD FLAGS(5);  /* R + X = 0x4 + 0x1 */
  data PT_LOAD FLAGS(6);  /* R + W = 0x4 + 0x2 */
}

SECTIONS {
	. = 0x10000;
	
	.start : {
    *(.start)
	} :text
	
	.text :
	{
		*(.text*)
	}:text
	
	.rodata :
    {
        *(.rodata*)
    }:text
	
    .data :
    {
        *(.data*)
    }:data
	
    .bss :
    {
        *(.bss*)
    }:data
}