#!/bin/bash

clang --target=i686-elf -ffreestanding -fno-stack-protector -c kernel.c -o kernel.o || exit 1

ld.lld -m elf_i386 -T linker.ld boot.o kernel.o -o ovics.elf || exit 1

/c/msys64/ucrt64/bin/qemu-system-i386.exe -kernel ovics.elf