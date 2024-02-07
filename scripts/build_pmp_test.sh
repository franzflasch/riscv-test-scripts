#!/bin/bash

compile_pmp_user_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"

    mkdir -p ${output_dir}
    local tests_dir="pmp/userspace_test_programs"
    local env_dir="riscv-tests/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -march=rv${arch}ima_zicsr -mcmodel=medany -g \
        -Wl,-T,${tests_dir}/user.ld,-Bstatic -ffreestanding -nostartfiles -specs=nano.specs \
        -Wl,--no-warn-rwx-segment \
        -o ${output_dir}/${test_name}_${output_file}.elf \
        ${tests_dir}/init.S ${tests_dir}/${test_name}.c

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin

    xxd -i -a ${output_dir}/${test_name}_${output_file}.bin > ${output_dir}/${test_name}_${output_file}.c
}

compile_pmp_func()
{
    local arch="$1"
    local test_name="$2"
    local output_dir="$3"

    mkdir -p ${output_dir}
    local tests_dir="pmp/"
    local env_dir="riscv-tests/env"
    local output_file="rv${arch}"

    riscv${arch}-none-elf-gcc -march=rv${arch}ima_zicsr -mcmodel=medany -g \
        -Wl,-T,${tests_dir}/test.ld,-Bstatic -ffreestanding -nostartfiles -specs=nano.specs \
        -Wl,--no-warn-rwx-segment \
        -o ${output_dir}/${test_name}_${output_file}.elf \
        ${tests_dir}/init.S ${tests_dir}/${test_name}.c

    riscv${arch}-none-elf-objcopy -O binary \
        ${output_dir}/${test_name}_${output_file}.elf \
        ${output_dir}/${test_name}_${output_file}.bin
}
