ORG 0x10000
BITS 32

boot_info_arr equ 0x900
idtr equ 0x12100
init_idt equ 0x12200
initialize_keyboard equ 0x12300
kernel_start equ 0x12500

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
