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
mov ax, 0x2000
mov es, ax
mov bx, 0

; init configs for reading from disk
mov ah, 02h
mov al, 0x40
mov ch, 0
mov cl, 07h
mov dh, 0
mov dl, 0

; interrupt for reading from disk
read_disk:
	int 13h
	or ah, ah
	jnz read_disk

PRE_GDT:
	; enable A20
	wait_keyboard_c_input:
			in al, 0x64
			test al, 2
			jnz wait_keyboard_c_input
	mov al, 0xd1
	out 0x64, al
	wait_keyboard_c_output:
			in al, 0x64
			test al, 2
			jnz wait_keyboard_c_output
	mov al, 0xDF
	out 0x60, al
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