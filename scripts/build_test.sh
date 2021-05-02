#!/bin/bash

compile_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"
    local extension="$4"

    mkdir -p ${output_dir}
    local tests_dir="riscv-tests/isa/rv${arch}${extension}"
    local env_dir="riscv-tests/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -march=rv${arch}ima -g \
        -I. -I${tests_dir}/../macros/scalar/ -I${env_dir}/ \
        -Wl,-T,linker_script.ld,-Bstatic -ffreestanding -nostdlib \
        ${tests_dir}/${test_name}.S \
        -o ${output_dir}/${test_name}_${output_file}.elf \

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin
}
