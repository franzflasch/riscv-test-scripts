#!/bin/bash

riscv32-none-elf-gcc -march=rv32ima -g \
    -Wl,-T,linker.ld,-Bstatic -ffreestanding -nostdlib \
    simple.S \
    -o simple.elf \

riscv32-none-elf-objcopy -O binary \
    simple.elf \
    simple.bin
