#!/bin/bash

set -e

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ARCH>" >&2
	exit 1
fi

ARCH="$1"

source "${BASH_SOURCE%/*}/tests.sh"

scripts/build_riscv_tests.sh ${ARCH}

QEMU_REG_STATES_DIR="generated_files/qemu_${ARCH}/register_states"

COMPILED_DIR="generated_files/rv${ARCH}/compiled_files"
TRACES_DIR="generated_files/riscv_em${ARCH}/traces"
REG_STATES_DIR="generated_files/riscv_em${ARCH}/register_states"

mkdir -p ${TRACES_DIR}
mkdir -p ${REG_STATES_DIR}

tests=("${tests_ui[@]}" "${tests_ua[@]}" "${tests_um[@]}" "${tests_my_tests[@]}" "${tests_my_c_tests[@]}" "${tests_compliance[@]}")
#tests=("${tests_my_tests[@]}" "${tests_my_c_tests[@]}")
#tests=("${tests_ua[@]}")
#tests=("${tests_my_tests[@]}")
#tests=("slliw")
#tests=("${tests_compliance[@]}")

MAX_NUM_CYCLES=150000

EXIT_CODE=0
tmp_local_exit_code=0

for i in "${tests[@]}"
do
    SUCCESS_PC="$(riscv${ARCH}-none-elf-objdump -S ${COMPILED_DIR}/${i}_rv${ARCH}.elf | grep "<pass>:" | awk '{print $1}')"
    #echo ${SUCCESS_PC} ${i}_rv${ARCH}.bin
    set +e
    riscv_em -f ${COMPILED_DIR}/${i}_rv${ARCH}.bin -s ${SUCCESS_PC} -n ${MAX_NUM_CYCLES} > ${TRACES_DIR}/${i}.txt
    set -e

    scripts/convert_riscv_em_output.sh ${TRACES_DIR}/${i}.txt > ${REG_STATES_DIR}/${i}.txt
    cmp --silent ${REG_STATES_DIR}/${i}.txt ${QEMU_REG_STATES_DIR}/${i}_states.txt && tmp_local_exit_code=0 || tmp_local_exit_code=1

    if [ ${tmp_local_exit_code} -eq 0 ]; then
        echo "### SUCCESS: ${i}_states.txt Are Identical! ###"
    else
        echo "### ERROR: ${i}_states.txt Are Different! ###"
        EXIT_CODE=1
    fi
done

exit ${EXIT_CODE}
