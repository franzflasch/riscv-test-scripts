#!/bin/bash

set -e

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ARCH>" >&2
	exit 1
fi

ARCH="$1"

done_file=generated_files/rv${ARCH}/done
if [[ -f "${done_file}" ]]; then
    echo "Files already built"
    exit 0;
fi

source "${BASH_SOURCE%/*}/../tests.sh"

mkdir -p generated_files/rv${ARCH}/compiled_files

source scripts/build_test.sh
for i in "${tests_ui[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files ui
done

for i in "${tests_ua[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files ua
done

for i in "${tests_um[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files um
done

for i in "${tests_mi[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files mi
done

source scripts/build_my_test.sh
for i in "${tests_my_tests[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_my_c_tests[@]}"
do
    echo "Building ${i}"
    compile_c_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

source scripts/build_compliance_test.sh
for i in "${tests_compliance[@]}"
do
    echo "Building ${i}"
    compile_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files ui
done

source scripts/build_pmp_test.sh
for i in "${tests_pmp_user[@]}"
do
    echo "Building ${i}"
    compile_pmp_user_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_pmp[@]}"
do
    echo "Building ${i}"
    compile_pmp_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

source scripts/build_priv_test.sh
for i in "${tests_priv_user[@]}"
do
    echo "Building ${i}"
    compile_priv_user_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_priv_supervisor[@]}"
do
    echo "Building ${i}"
    compile_priv_supervisor_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_priv[@]}"
do
    echo "Building ${i}"
    compile_priv_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

source scripts/build_mmu_test.sh
for i in "${tests_mmu_user[@]}"
do
    echo "Building ${i}"
    compile_mmu_user_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_mmu_supervisor[@]}"
do
    echo "Building ${i}"
    compile_mmu_supervisor_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

for i in "${tests_mmu[@]}"
do
    echo "Building ${i}"
    compile_mmu_func ${ARCH} $i generated_files/rv${ARCH}/compiled_files
done

touch generated_files/rv${ARCH}/done
