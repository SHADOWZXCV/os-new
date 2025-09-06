@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

:parse_args
set "debugging=false"
for %%i in (%*) do (
    if "%%i"=="-g" (
        set "debugging=true"
    )
)

echo Deleting old files...
del kernel.bin
del kernel.elf
del os-image.bin
del os.img
del kernel_elf_sections.txt
del kernel_elf_symbols.txt
del boot1.bin
del boot2.bin
del bootloader.bin
del qemu_log.txt

echo Assembling bootloader...
nasm -f bin boot.asm -l boot1.lst -o boot1.bin || exit /b 1
nasm -f bin boot2.asm -l boot2.lst -o boot2.bin || exit /b 1
cat boot1.bin boot2.bin > bootloader.bin || exit /b 1

echo Compiling and linking kernel...
i686-elf-gcc -c main.c -o build/main.o -ffreestanding -m32 -O2 -nostdlib -g || exit /b 1
i686-elf-gcc -c start.c -o build/start.o -ffreestanding -m32 -O2 -nostdlib -g || exit /b 1
i686-elf-ld -T linker.ld build/start.o build/main.o -o kernel.elf -Map kernel.map || exit /b 1
i686-elf-objcopy -O binary kernel.elf kernel.bin || exit /b 1

echo Creating OS image...
cat bootloader.bin kernel.bin > os-image.bin || exit /b 1
cp os-image.bin os.img || exit /b 1

if "%debugging%"=="true" (
    echo Starting QEMU in debugging mode...
    qemu-system-i386 -fda os.img -M pc -S -s || exit /b 1
) else (
    echo Starting QEMU...
    qemu-system-i386 -fda os.img -M pc -D qemu_log.txt -d cpu -no-shutdown -no-reboot || exit /b 1
)

