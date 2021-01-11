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

for i in "${tests_ui[@]}"
do
    scripts/build_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} ui
done

for i in "${tests_ua[@]}"
do
    scripts/build_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} ua
done

for i in "${tests_um[@]}"
do
    scripts/build_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} um
done

for i in "${tests_mi[@]}"
do
    scripts/build_p_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} mi
done

for i in "${tests_my_tests[@]}"
do
    scripts/build_my_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH}
done

for i in "${tests_my_c_tests[@]}"
do
    scripts/build_my_test.sh $i mytests/c_env.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} "C"
done

for i in "${tests_compliance[@]}"
do
    scripts/build_compliance_test.sh $i linker_script.ld rv${ARCH} generated_files/rv${ARCH}/compiled_files ${ARCH} ui
done

touch generated_files/rv${ARCH}/done
