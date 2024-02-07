#!/bin/bash

compile_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"

    mkdir -p ${output_dir}
    local tests_dir="mytests/"
    local env_dir="riscv-tests/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -march=rv${arch}ima_zicsr -g \
        -I. -I${tests_dir}/../macros/scalar/ -I${env_dir}/ \
        -Wl,-T,linker_script.ld,-Bstatic -ffreestanding -nostdlib \
        -Wl,--no-warn-rwx-segment \
        ${tests_dir}/${test_name}.S \
        -o ${output_dir}/${test_name}_${output_file}.elf \

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin
}

compile_c_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"

    mkdir -p ${output_dir}
    local tests_dir="mytests/"
    local env_dir="riscv-tests/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -march=rv${arch}ima_zicsr -mcmodel=medany -g \
        -Wl,-T,mytests/c_env.ld,-Bstatic -ffreestanding -nostartfiles -specs=nano.specs \
        -Wl,--no-warn-rwx-segment \
        -o ${output_dir}/${test_name}_${output_file}.elf \
        ${tests_dir}/init.S ${tests_dir}/${test_name}.c

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin
}
