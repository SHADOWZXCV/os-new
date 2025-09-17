extern keyboard_handler
extern timer_handler
extern eisr_handler_no_details

idt_start:
	%assign i 0
	%rep 256
		dq 0
		%assign i i+1
	%endrep
idt_end:

section .idtr
idtr:
	dw idt_end - idt_start - 1
	dd idt_start

section .init_idt
init_idt:
	mov eax, keyboard_isr
	mov word [idt_start + 0x21 * 8 + 0], ax ; low offset
	mov word [idt_start + 0x21 * 8 + 2], 0x08 ; code segment of kernel
	mov byte [idt_start + 0x21 * 8 + 4], 0 ; reserved
	mov byte [idt_start + 0x21 * 8 + 5], 0x8E ; 32-bit interrupt gate, and present bit
	shr eax, 16
	mov word [idt_start + 0x21 * 8 + 6], ax ; high offset of keyboard_isr
	mov eax, EISR_NODETAILS
	mov word [idt_start + 0], ax ; low offset
	mov word [idt_start + 2], 0x08 ; code segment of kernel
	mov byte [idt_start + 4], 0 ; reserved
	mov byte [idt_start + 5], 0x8E ; 32-bit interrupt gate, and present bit
	shr eax, 16
	mov word [idt_start + 6], ax ; high offset of div_by_zero
	mov eax, clock_timer_isr
	mov word [idt_start + 0x28 * 8 + 0], ax ; low offset
	mov word [idt_start + 0x28 * 8 + 2], 0x08 ; code segment of kernel
	mov byte [idt_start + 0x28 * 8 + 4], 0 ; reserved
	mov byte [idt_start + 0x28 * 8 + 5], 0x8E ; 32-bit interrupt gate, and present bit
	shr eax, 16
	mov word [idt_start + 0x28 * 8 + 6], ax ; high offset of clock_timer_isr
ret

clock_timer_isr:
	pusha
	call timer_handler
	mov al, 0x0C
	out 0x70, al
	in al, 0x71

	; EOI
	mov al, 0x20
	out 0xA0, al
	out 0x20, al
	popa
iret

keyboard_isr:
	pusha
	call keyboard_handler
	mov al, 0x20
	out 0x20, al  ; EOI
	popa
	iret
	; implement

section .initialize_keyboard
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

ret

EISR_NODETAILS:
    pusha
    push dword 0
    push dword 0
    call eisr_handler_no_details
    add esp, 8  ; since we pushed 2 4-byte arguments onto the stack, we need to skip them for the popa to work
    popa
iretd
