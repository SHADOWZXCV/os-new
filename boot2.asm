ORG 0x10000
BITS 32

keyboard_handler equ 0x20000
kernel_start equ 0x20100
idt_start:
	%assign i 0
	%rep 256
		dq 0
		%assign i i+1
	%endrep
idt_end:

idtr:
	dw idt_end - idt_start - 1
	dd idt_start

jmp init
	
keyboard_isr:
	pusha
	call keyboard_handler
	mov al, 0x20
	out 0x20, al  ; EOI
	popa
	iret
	; implement
	
; Remap PICs
init:
mov al, 0x11 ; ICW1, INIT AND ICW4
out 0x20, al
jmp $+2
out 0xA0, al
jmp $+2

; ICW 2
mov al, 0x20 ; base address of 32
out 0x21, al
jmp $+2

mov al, 0x28 ; base address of 40
out 0xA1, al
jmp $+2

; ICW 3
mov al, 0x4
out 0x21, al
jmp $+2

mov al, 0x2
out 0xA1, al
jmp $+2

; ICW 4
mov	al, 1		; bit 0 enables 80x86 mode
 
; send ICW 4 to both primary and secondary PICs
out	0x21, al
jmp $+2
out	0xA1, al
jmp $+2

; http://www.brokenthorn.com/Resources/OSDevPic.html

init_idt:
	mov eax, keyboard_isr
	mov word [idt_start + 0x21 * 8 + 0], ax ; low offset
	mov word [idt_start + 0x21 * 8 + 2], 0x08 ; code segment of kernel
	mov byte [idt_start + 0x21 * 8 + 4], 0 ; reserved
	mov byte [idt_start + 0x21 * 8 + 5], 0x8E ; 32-bit interrupt gate, and present bit
	shr eax, 16
	mov word [idt_start + 0x21 * 8 + 6], ax ; high offset of keyboard_isr

lidt [idtr]
sti
; unmask ( enable ) interrupts for master and slave
mov al, 0b11111101
out 0x21, al
mov al, 0xFF
out 0xA1, al

; print debug character "B" to video memory
mov edi, 0xB8000
mov byte [edi], 'B'
mov byte [edi + 1], 0x1B

jmp 08h:kernel_start ; far jump to kernel

times 2560 - ($ - $$) db 0 ; padding to ensure this file takes exactly a whole sector