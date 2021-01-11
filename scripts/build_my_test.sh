#!/bin/bash

if [ "$#" -lt 5 ]; then
	echo "Usage: $0 <test_name> <linker_script> <output_filename> <output_dir> <ARCH>" >&2
	echo "Example (leiwandrv32): ./build_test.sh addi link_leiwandrv32.ld leiwandrv32 test_output"
	echo "Example (qemu): ./build_test.sh addi link_qemu.ld qemu test_output"
	exit 1
fi

TEST_NAME="$1"
LINKER_SCRIPT="$2"
OUTPUT_FILE="$3"
OUTPUT_DIR="$4"
ARCH="$5"
IS_C_FILE="$6"

TESTS_DIR="mytests/"
ENV_DIR="riscv-tests/env"

mkdir -p $OUTPUT_DIR


if [ "$IS_C_FILE" == "C" ]; then
	riscv${ARCH}-none-elf-gcc -march=rv${ARCH}ia -mcmodel=medany -g \
		-Wl,-T,$LINKER_SCRIPT,-Bstatic -ffreestanding -nostartfiles -specs=nano.specs \
		-o ${OUTPUT_DIR}/${TEST_NAME}_${OUTPUT_FILE}.elf ${TESTS_DIR}/init.S ${TESTS_DIR}/${TEST_NAME}.c
else
	riscv${ARCH}-none-elf-gcc -march=rv${ARCH}ia -g \
		-I. -I${TESTS_DIR}/../macros/scalar/ -I${ENV_DIR}/ -Wl,-T,$LINKER_SCRIPT,-Bstatic -ffreestanding -nostdlib \
		-o ${OUTPUT_DIR}/${TEST_NAME}_${OUTPUT_FILE}.elf ${TESTS_DIR}/${TEST_NAME}.S
fi

riscv${ARCH}-none-elf-objcopy -O binary ${OUTPUT_DIR}/${TEST_NAME}_${OUTPUT_FILE}.elf ${OUTPUT_DIR}/${TEST_NAME}_${OUTPUT_FILE}.bin
