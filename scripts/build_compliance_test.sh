#!/bin/bash

compile_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"
    local extension="$4"

    mkdir -p ${output_dir}
    local tests_dir="riscv-compliance/riscv-test-suite/rv${arch}i_m/I/src"
    local env_dir="riscv-compliance/riscv-test-suite/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -DXLEN=${arch} -march=rv${arch}ima -g \
        -I. -I riscv-compliance/riscv-target/spike/ -I${tests_dir}/../macros/scalar/ -I${env_dir}/ \
        -Wl,-T,linker_script.ld,-Bstatic -ffreestanding -nostdlib \
        ${tests_dir}/${test_name}.S \
        -o ${output_dir}/${test_name}_${output_file}.elf

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin
}
