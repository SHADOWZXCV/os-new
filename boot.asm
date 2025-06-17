BITS 16
ORG 0x7C00

; read from disk
; init base and offset to which the kernel is loaded to
mov ax, 0
mov es, ax
mov bx, 0x1000

; init configs for reading from disk
mov ah, 02h
mov al, 02h
mov ch, 0
mov cl, 02h
mv dh, 0

; interrupt for reading from disk
int 13h
or ah, ah
jnz reset_drive
jmp PRE_GDT

reset_drive:
	mov ah, 0
	int 13h
	or ah, ah
	jnz reset_drive

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

PRE_GDT:
	cli ; disable interrupts
	lgdt [gdtr]
	; enable protected mode bit in CR0, copies cr0, sets bit 0, then adds it back
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	jmp 08h:enable_protected
	
BITS 32
enable_protected:
		mov ax, 0x10
		mov ds, ax
		mov ss, ax
		mov esp, 090000h
		
		; print characters onto screen
;		mov edi, 0xB8000
;		mov byte [edi], 'A'
;		mov byte [edi + 1], 0x1B
; hang:
;   jmp hang
		jmp 08h:01000h ; main
times 510-($-$$) db 0

dw 0xAA55