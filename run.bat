del kernel.bin
del kernel.elf
del os-image.bin
del os.img
del kernel_elf_sections.txt
del kernel_elf_symbols.txt
del boot1.bin
del boot2.bin
del kernel.elf
del os.imgs
del bootloader.bin
del qemu_log.txt


nasm -f bin boot.asm -l boot1.lst -o boot1.bin
nasm -f bin boot2.asm -l boot2.lst -o boot2.bin
cat boot1.bin boot2.bin > bootloader.bin
i686-elf-gcc -c main.c -o build/main.o -ffreestanding -m32 -O2 -nostdlib -g
i686-elf-gcc -c start.c -o build/start.o -ffreestanding -m32 -O2 -nostdlib -g
i686-elf-ld -T linker.ld build/start.o build/main.o -o kernel.elf -Map kernel.map
i686-elf-objcopy -O binary kernel.elf kernel.bin
cat bootloader.bin kernel.bin > os-image.bin
cp os-image.bin os.img
qemu-system-i386 -fda os.img -M pc -D qemu_log.txt -d cpu -no-shutdown -no-reboot
@REM qemu-system-i386 -fda os.img -M pc -S -s