#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ARCH>" >&2
	exit 1
fi

ARCH="$1"

done_file=generated_files/qemu_${ARCH}/done
if [[ -f "${done_file}" ]]; then
    echo "State Files already generated"
    exit 0;
fi

source "${BASH_SOURCE%/*}/tests.sh"

COMPILED_DIR="generated_files/rv${ARCH}/compiled_files"
TRACES_DIR="generated_files/qemu_${ARCH}/traces"
REG_STATES_DIR="generated_files/qemu_${ARCH}/register_states"

mkdir -p ${TRACES_DIR}
mkdir -p ${REG_STATES_DIR}

scripts/build_riscv_tests.sh ${ARCH}

tests=("${tests_ui[@]}" "${tests_ua[@]}" "${tests_um[@]}" "${tests_my_tests[@]}" "${tests_my_c_tests[@]}" "${tests_compliance[@]}" "${tests_pmp[@]}" "${tests_priv[@]}")
#tests=("${tests_my_tests[@]}" "${tests_my_c_tests[@]}")
#tests=("${tests_mmu[@]}")
#tests=("${tests_ui[@]}")

for i in "${tests[@]}"
do
    SUCCESS_PC="$(riscv${ARCH}-none-elf-objdump -S ${COMPILED_DIR}/${i}_rv${ARCH}.elf | grep "<pass>:" | awk '{print $1}')"
    echo $SUCCESS_PC
    END_PC=$(echo $((16#$SUCCESS_PC)))
    echo $END_PC

    qemu-system-riscv${ARCH} -nographic -machine virt -m 128M -bios none -kernel ${COMPILED_DIR}/${i}_rv${ARCH}.elf -d nochain,cpu -singlestep -s -S 2> ${TRACES_DIR}/${i}_trace.txt &

    # sleep here to give qemu time to start up
    sleep 1

    riscv${ARCH}-none-elf-gdb -ex "set arch riscv:rv${ARCH}" -ex "target remote localhost:1234" -ex "source step_mult.gdb" -ex "b *0x${SUCCESS_PC}" -ex "c" -ex "set confirm off" -ex "q" ${COMPILED_DIR}/${i}_rv${ARCH}.elf

    kill -9 $(pgrep -f "qemu-system-riscv${ARCH}")

    # sleep here to give linux some time to kill
    sleep 1
done

for i in "${tests[@]}"
do
    SUCCESS_PC="$(riscv${ARCH}-none-elf-objdump -S ${COMPILED_DIR}/${i}_rv${ARCH}.elf | grep "<pass>:" | awk '{print $1}')"
    scripts/convert_qemu_output.sh ${TRACES_DIR}/${i}_trace.txt > ${REG_STATES_DIR}/${i}_states.txt
    # delete first five lines as this is some jump in qemu which is not in the elf
    # sed -i -e '1,5d' qemu_${ARCH}/register_states/${i}_states.txt

    # now delete all lines after the success symbol, as qemu runs freely after we quit gdb
    last_line_number=$(cat ${REG_STATES_DIR}/${i}_states.txt | awk '{print $1}' | grep -n $SUCCESS_PC | cut -d : -f 1 | head -n1)
    last_line_number=$((last_line_number+1))
    sed -i ${last_line_number}',$d' ${REG_STATES_DIR}/${i}_states.txt
done

touch generated_files/qemu_${ARCH}/done
