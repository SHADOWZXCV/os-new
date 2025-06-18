@REM nasm boot.asm -f bin -o bootloader.bin
@REM i686-elf-gcc -c main.c -o main.o -ffreestanding -g
@REM i686-elf-ld -e main -Ttext 0x10000 main.o -o kernel.o
@REM i686-elf-ld -i -e main -Ttext 0x10000 main.o -o kernel.o
@REM i686-elf-objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin


@REM nasm boot.asm -f bin -o bootloader.bin
@REM i686-elf-gcc -c main.c -o main.o -ffreestanding -O2 -Wall -nostdlib -m32 -g
@REM i686-elf-ld -T linker.ld main.o -o kernel.elf
@REM i686-elf-ld -T linker.ld main.o -o kernel.bin --oformat binary
@REM i686-elf-ld -T linker.ld main.o -o kernel.elf
@REM i686-elf-objcopy -O binary kernel.elf kernel.bin


nasm boot.asm -f bin -o bootloader.bin
i686-elf-gcc -c main.c -o build/main.o -ffreestanding -m32 -O2 -nostdlib
i686-elf-gcc -c start.c -o build/start.o -ffreestanding -m32 -O2 -nostdlib
i686-elf-ld -T linker.ld build/start.o build/main.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
cat bootloader.bin kernel.bin > os-image.bin
cp os-image.bin os.img
qemu-system-i386 -fda os.img -M pc -D qemu_log.txt -d cpu -no-shutdown -no-reboot
