BITS 16
ORG 0x7C00

stage2_start equ 0x10000

start:
	cli ; disable interrupts for protected mode later
	jmp load_stage_2

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
	
load_stage_2:
	mov ax, 0x1000
	mov es, ax
	mov bx, 0x0000
	
	mov ah, 0x02       ; BIOS read sector function
	mov al, 5          ; number of sectors to read
	mov ch, 0          ; cylinder
	mov cl, 2          ; sector number = 2 (sector numbering starts from 1)
	mov dh, 0          ; head
	mov dl, 0x00       ; drive number (0x00 = floppy, 0x80 = HDD)

	read_disk_stage_2:
		int 13h
		or ah, ah
		jnz read_disk_stage_2

init_disk_read_kernel: ; read from disk
	; init base and offset to which the kernel is loaded to
	mov ax, 0x1200
	mov es, ax
	mov bx, 0

	; init configs for reading from disk
	mov ah, 02h
	mov al, 0x20
	mov ch, 0
	mov cl, 07h
	mov dh, 0
	mov dl, 0

	; interrupt for reading from disk
	read_disk:
		int 13h
		or ah, ah
		jnz read_disk

jmp PRE_GDT

check_a20:
	pushf
	push ds
	push es
	push di
	push si

	cli

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
	sti
	mov ax, 0
	jne end_check_a20
	mov ax, 1
end_check_a20:	
	pop si
    pop di
    pop es
    pop ds
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
	; =================================== ;
	; PROTECTED MODE ENABLE				  ;
	; =================================== ;
	lgdt [gdtr]
	; enable protected mode bit in CR0, copies cr0, sets bit 0, then adds it back
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	jmp 08h:enable_protected
	
BITS 32
enable_protected:
		mov ax, 0x10 ; 0001 0000 ( code segment )
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax
		mov esp, 090000h ; stack place ( away from code segment )
		
		
; print characters onto screen
mov edi, 0xB8000
mov byte [edi], 'A'
mov byte [edi + 1], 0x1B

jmp 08h:stage2_start ; stage 2

times 510-($-$$) db 0

dw 0xAA55