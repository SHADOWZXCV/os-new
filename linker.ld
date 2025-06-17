ENTRY(kernel_main)

SECTIONS {
	.=0x10000
	
	.text: AT(0x10000) {
		*(.multiboot)
		*(.text)
		.= ALIGN(4096);
	}
	
	 .rodata : AT(ADDR(.text) + SIZEOF(.text))
    {
        *(.rodata)
        . = ALIGN(4096);
    }

    .data : AT(ADDR(.rodata) + SIZEOF(.rodata))
    {
        *(.data)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.data) + SIZEOF(.data))
    {
        *(.bss)
        . = ALIGN(4096);
    }
}