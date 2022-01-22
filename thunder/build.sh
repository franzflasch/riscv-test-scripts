#!/bin/bash

programs=( simple simple2 simple3 simple4 simple6 simple9 simple10 )

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

c_programs=( simple5 simple7 simple8 )

for i in "${c_programs[@]}"
do
    riscv32-none-elf-gcc -march=rv32ima -g \
        -Wl,-T,linker.ld,-Bstatic -fdump-tree-cfg-lineno-graph -ffreestanding -nostdlib \
        ${i}.c

    riscv32-none-elf-gcc -march=rv32ima -g \
        -Wl,-T,linker.ld,-Bstatic -ffreestanding -nostdlib \
        ${i}.c \
        -o ${i}.elf \

    riscv32-none-elf-objcopy -O binary \
        ${i}.elf \
        ${i}.bin
done
