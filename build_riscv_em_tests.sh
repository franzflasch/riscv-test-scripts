#!/bin/bash

set -e

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ARCH>" >&2
	exit 1
fi

ARCH="$1"

source "${BASH_SOURCE%/*}/tests.sh"

./build_riscv_tests.sh ${ARCH}

QEMU_REG_STATES_DIR="qemu_generated_${ARCH}/qemu_register_states"

COMPILED_DIR="rv${ARCH}_generated/compiled_files"
TRACES_DIR="riscv_em${ARCH}_generated/riscv_em_traces"
REG_STATES_DIR="riscv_em${ARCH}_generated/riscv_em_register_states"

mkdir -p ${TRACES_DIR}
mkdir -p ${REG_STATES_DIR}

tests=("${tests_ui[@]}" "${tests_ua[@]}" "${tests_um[@]}" "${tests_my_tests[@]}" "${tests_my_c_tests[@]}" "${tests_compliance[@]}")
#tests=("${tests_my_tests[@]}" "${tests_my_c_tests[@]}")
#tests=("${tests_ua[@]}")
#tests=("${tests_my_tests[@]}")
#tests=("slliw")
#tests=("${tests_compliance[@]}")

MAX_NUM_CYCLES=150000

for i in "${tests[@]}"
do
    SUCCESS_PC="$(riscv${ARCH}-none-elf-objdump -S rv${ARCH}_generated/compiled_files/${i}_rv${ARCH}.elf | grep "<pass>:" | awk '{print $1}')"
    #echo ${SUCCESS_PC} ${i}_rv${ARCH}.bin
    set +e
    riscv_em -f ${COMPILED_DIR}/${i}_rv${ARCH}.bin -s ${SUCCESS_PC} -n ${MAX_NUM_CYCLES} > ${TRACES_DIR}/${i}.txt -d /working_dir/Desktop/work/private/fpga/riscv_em/dts/riscv_em.dtb
    set -e

    ./convert_riscv_em_output.sh ${TRACES_DIR}/${i}.txt > ${REG_STATES_DIR}/${i}.txt
    cmp --silent ${REG_STATES_DIR}/${i}.txt ${QEMU_REG_STATES_DIR}/${i}_states.txt && echo "### SUCCESS: ${i}_states.txt Are Identical! ###" || echo "### ERROR: ${i}_states.txt Are Different! ###"
done
