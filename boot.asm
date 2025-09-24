BITS 16
ORG 0x7C00

stage2_start equ 0x1000
call start

read_disk_interrupt:
	int 13h
	or ah, ah
	jnz read_disk_interrupt
ret

start:
	cli ; disable interrupts for protected mode later
	
load_stage_2:
	mov ax, 0x100
	mov es, ax
	mov bx, 0x0000
	
	mov ah, 0x02       ; BIOS read sector function
	mov al, 5          ; number of sectors to read
	mov ch, 0          ; cylinder
	mov cl, 2          ; sector number = 2 (sector numbering starts from 1)
	mov dh, 0          ; head
	mov dl, 0x00       ; drive number (0x00 = floppy, 0x80 = HDD)

	call read_disk_interrupt

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
	call read_disk_interrupt

; print debug character - indicating success of stage 1
mov edi, 0xB8000
mov byte [edi], 'A'
mov byte [edi + 1], 0x1B

jmp stage2_start ; stage 2

times 510-($-$$) db 0
dw 0xAA55
