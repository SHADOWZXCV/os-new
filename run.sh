#!/bin/bash
set -e  # Exit on any error
set -u  # Treat unset variables as errors

# Parse arguments
DEBUGGING=false
for arg in "$@"; do
    if [ "$arg" == "-g" ]; then
        DEBUGGING=true
    fi
done

echo "Deleting old files..."
rm -f kernel.bin kernel.elf os-image.bin os.img \
      kernel_elf_sections.txt kernel_elf_symbols.txt \
      boot1.bin boot2.bin bootloader.bin qemu_log.txt

echo "Assembling bootloader..."
nasm -f bin boot.asm -l boot1.lst -o boot1.bin
nasm -f bin boot2.asm -l boot2.lst -o boot2.bin
cat boot1.bin boot2.bin > bootloader.bin

echo "Compiling and linking kernel..."
# mkdir -p build
# gcc -c main.c -o build/main.o -ffreestanding -m32 -O2 -nostdlib -g -O0 -fno-omit-frame-pointer
# gcc -c start.c -o build/start.o -ffreestanding -m32 -O2 -nostdlib -g -O0 -fno-omit-frame-pointer
# ld -m elf_i386 -T linker.ld build/start.o build/main.o -o kernel.elf -Map kernel.map
make all
objcopy -O binary kernel.elf kernel.bin

echo "Creating OS image..."
cat bootloader.bin kernel.bin > os-image.bin
cp os-image.bin os.img

if [ "$DEBUGGING" = true ]; then
    echo "Starting QEMU in debugging mode..."
    qemu-system-i386 -fda os.img -S -s -display gtk -m 2048
else
    echo "Starting QEMU..."
    qemu-system-i386 -fda os.img -M pc -display gtk -m 2048
fi
