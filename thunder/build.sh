#!/bin/bash

programs=( simple simple2 simple3 simple4 )

for i in "${programs[@]}"
do
    riscv32-none-elf-gcc -march=rv32ima -g \
        -Wl,-T,linker.ld,-Bstatic -ffreestanding -nostdlib \
        ${i}.S \
        -o ${i}.elf \

    riscv32-none-elf-objcopy -O binary \
        ${i}.elf \
        ${i}.bin
done
