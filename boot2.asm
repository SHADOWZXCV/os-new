ORG 0x1000
BITS 16
struc boot_info
    memory_low   resw 1
    memory_high  resw 1
    m_map_add    resw 1
    m_map_off    resw 1
    m_map_length resw 1
endstruc
CODE_SEG_32 equ 0x08

call BIOS_get_mem_size
; this is the struct I need on my kernel, the multiboot info one
; define words and dwords here eventually with the values I have on ax, and bx
; and put them on the memoryHi + memoryLo
mov [boot_info_arr], ax
mov [boot_info_arr + 2], bx
; then I call:
call BIOS_get_memory_info
mov [boot_info_arr + 4], es
mov word [boot_info_arr + 6], 0
mov [boot_info_arr + 8], bp
; then I get the es:di position, and the length from bp, and I put them somewhere 
; so that my main kernel entry when called, I can push the boot_info address on
; the stack, and have access to all this info ?
; continue the rest of the program

jmp PRE_GDT

BIOS_get_mem_size:
	mov ax, 0
    mov es, ax          ; segment
    mov di, 0x0            ; (initial) offset
    mov ebx, 0x0         ; (initial) continuation value
	xor	ecx, ecx		;clear all registers. This is needed for testing later
	xor	edx, edx
	mov	ax, 0xe801
	int	0x15
	jc	error_mem_size
	cmp	ah, 0x86		;unsupported function
	je	error_mem_size
	cmp	ah, 0x80		;invalid command
	je	error_mem_size
	jcxz use_ax			;bios may have stored it in ax,bx or cx,dx. test if cx is 0
	mov	ax, cx			;its not, so it should contain mem size; store it
	mov	bx, dx
 
    use_ax:
        ret
 
    error_mem_size:
        mov	ax, -1
        mov	bx, 0
ret

BIOS_get_memory_info: 
	mov ax, 0x910
	mov es, ax
	mov di, 0
    xor ebx, ebx
    xor bp, bp
    ; INPUT
    mov edx, 'PAMS'
    mov eax, 0xe820
    mov ecx, 24

    int 0x15

    jc error_mem_map

    cmp eax, 'PAMS'
    jne error_mem_map

    test ebx, ebx ; there's no way we're done!
    je error_mem_map

    start_mmap:
        jcxz continue

        mov ecx, [es:di + 8]
        test ecx, ecx
        jne nice
        mov ecx, [es:di + 12]
        jecxz continue

    nice:
        inc bp
        add di, 24
    continue:
        test ebx, ebx
        je finish
        mov edx, 'PAMS'
        mov ecx, 24
        mov eax, 0xe820
        int 0x15
        jmp start_mmap
    error_mem_map:
        stc
    finish:
ret


gdt_start:
	dq 0x0000000000000000 ; entry 1 is null
	; code segment
	dw 0xFFFF
	dw 0x0000 ; base pt-1
	db 0x00   ; base pt-2
	db 0b10011011 ; access byte
	db 0b11001111 ; flags+limit
	db 0x00
	; data segment
	dw 0xFFFF
	dw 0x0000 ; base pt-1
	db 0x00   ; base pt-2
	db 0b10010011 ; access byte
	db 0b11001111 ; flags+limit
	db 0x00
gdt_end:
	
gdtr:
	dw (gdt_end - gdt_start) - 1
	dd gdt_start

check_a20:
	pushf
	pusha
	push ds
	push es

	; cli

	xor ax, ax
	mov es, ax	; 0x0000

	not ax
	mov ds, ax	; 0XFFFF

	mov si, 0x500
	mov di, 0x510

	mov al, byte [es:si]
	push ax

	mov al, byte [ds:di]
	push ax

	mov byte [es:si], 0x00
	mov byte [ds:di], 0xFF

	cmp byte [es:si], 0xFF

	pop ax
	mov byte [ds:di], al
	
	pop ax
	mov byte [es:si], al
	; sti
	mov ax, 0
	jne end_check_a20
	mov ax, 1
end_check_a20:	
    pop es
    pop ds
	popa
    popf
ret

enable_a20:
	call check_a20
	cmp ax, 1
	je end_enable_a20
	; Enable A20, the BIOS  function
	mov ax, 0x2403
	int 15h
	jb enable_a20_keyboard_method
	cmp ah, 0
	jnz enable_a20_keyboard_method

	mov ax, 0x2402
	int 15h
	jb enable_a20_keyboard_method
	cmp ah, 0
	jnz enable_a20_keyboard_method

	cmp al, 1
	jnz enable_a20_keyboard_method

	mov ax, 0x2401
	int 15h
	jb enable_a20_keyboard_method
	cmp ah, 0
	jnz enable_a20_keyboard_method

	; check again the A20 line
	call check_a20
	cmp ax, 1
	je end_enable_a20

	; Enable A20, keyboard method
	enable_a20_keyboard_method:
		wait_keyboard_c_input:
				in al, 0x64						; read status of PS/2
				test al, 0b10					; test if bit 1 is set, meaning the buffer is clear
				jnz wait_keyboard_c_input
		mov al, 0xd1							; Write the next byte to the output port
		out 0x64, al							; Command: Tell the PS/2 controller to wait for a byte I am writing next
		wait_keyboard_c_output:
				in al, 0x64
				test al, 0b10
				jnz wait_keyboard_c_output
		mov al, 0xDF
		out 0x60, al							; Write the byte DF to the PS/2 data port
		call check_a20
		cmp ax, 1
		je end_enable_a20
	; Enable using fast A20
	in al, 0x92
	test al, 2
	jnz end_enable_a20
	or al, 2
	and al, 0xFE
	out 0x92, al
	mov ax, 0xFFFF
timeout_loop:
	dec ax
	jz end_enable_a20
	jmp timeout_loop
end_enable_a20:	ret

PRE_GDT:
	call enable_a20
	; enable the RTC interrupt
	push ax
	mov al, 0x8B
	out 0x70, al
	mov al, 0x20
	out 0x71, al
	pop ax
	; =================================== ;
	; PROTECTED MODE ENABLE				  ;
	; =================================== ;
	lgdt [gdtr]
	; enable protected mode bit in CR0, copies cr0, sets bit 0, then adds it back
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	jmp CODE_SEG_32:enable_protected
	
BITS 32
boot_info_arr equ 0x500
idtr equ 0x12100
init_idt equ 0x12200
initialize_keyboard equ 0x12300
kernel_start equ 0x12500

enable_protected:
		mov ax, 0x10 ; 0001 0000 ( code segment )
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov esp, 090000h ; stack place ( away from code segment )

call initialize_keyboard

; set up 8253 PIT chip
mov al, 110110b
out 0x43, al
mov bx, 1193180 / 100

mov ax, bx
out 0x40, al
xchg ah, al
out 0x40, al


; Remap PICs
; http://www.brokenthorn.com/Resources/OSDevPic.html
init_pic:
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

call init_idt

lidt [idtr]
sti
; unmask ( enable ) interrupts for master and slave
; keyboard + RTC + slave's IRQ2
mov al, 0b11111000
out 0x21, al
mov al, 0b11111110
out 0xA1, al

; print debug character "B" to video memory
mov edi, 0xB8000
mov byte [edi], 'B'
mov byte [edi + 1], 0x1B

mov eax, 0x2BADB002
mov ebx, boot_info_arr
jmp 08h:kernel_start ; far jump to kernel

times 2560 - ($ - $$) db 0 ; padding to ensure this file takes exactly 4 whole sectors
