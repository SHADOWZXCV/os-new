ORG 0x10000
BITS 32

keyboard_handler equ 0x20000
kernel_start equ 0x20200
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

jmp initialize_keyboard
	
keyboard_isr:
	pusha
	call keyboard_handler
	mov al, 0x20
	out 0x20, al  ; EOI
	popa
	iret
	; implement

; set keyboard scancode set to 2 does not work
initialize_keyboard:
disable_translation:
k_w_1:
    in    al, 0x64
    test  al, 0x02
    jnz   k_w_1

	mov al, 0x20 				; config byte
	out 0x64, al

k_w_2:
	in   al, 0x64
	test al, 0x01
	jz	k_w_2

	in  al, 0x60

	and al, 0xBF	; clear bit 6 ( translation bit )
	mov bl, al

k_w_3:
	in  al, 0x64
	test al, 2
	jnz k_w_3

	mov al, 0x60
	out 0x64, al

k_w_4:
	in al, 0x64
	test al, 2
	jnz k_w_4

	mov al, bl

	out 0x60, al

code_set_2:
	cli
    ; ────────────────
    ; Step 1: tell the keyboard we’re changing scan‑code set
    ; ────────────────

.wait_out_ready1:
    in    al, 0x64              ; read controller status
    test  al, 0x02              ; input‑buffer‑full bit?
    jnz   .wait_out_ready1      ; if 1, still busy → wait

	mov   al, 0xF0				; “Set Scan Code Set” command
    out   0x60, al              ; send 0xF0 to the keyboard

.wait_in_ready1:
    in    al, 0x64              ; read status again
    test  al, 0x01              ; output‑buffer‑full bit?
    jz    .wait_in_ready1       ; if 0, no data → wait

    in    al, 0x60              ; read ACK
    cmp   al, 0xFA
    jne   .wait_in_ready1   ; if not ACK, wait again

    ; ────────────────
    ; Step 2: send “2” (the desired set number)
    ; ────────────────

.wait_out_ready2:
    in    al, 0x64
    test  al, 0x02
    jnz    .wait_out_ready2

	mov   al, 2
	out   0x60, al

.wait_in_ready2:
    in    al, 0x64
	test  al, 0x01
	jz   .wait_in_ready2

	in	  al, 0x60
    cmp   al, 0xFA
    jne   .wait_in_ready2

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

times 2560 - ($ - $$) db 0 ; padding to ensure this file takes exactly 4 whole sectors
