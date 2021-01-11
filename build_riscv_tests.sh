#!/bin/bash

set -e

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ARCH>" >&2
	exit 1
fi

ARCH="$1"

done_file=rv${ARCH}_generated/done
if [[ -f "${done_file}" ]]; then
    echo "Files already built"
    exit 0;
fi

source "${BASH_SOURCE%/*}/tests.sh"

mkdir -p rv${ARCH}_generated/compiled_files

for i in "${tests_ui[@]}"
do
    ./build_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} ui
done

for i in "${tests_ua[@]}"
do
    ./build_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} ua
done

for i in "${tests_um[@]}"
do
    ./build_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} um
done

for i in "${tests_mi[@]}"
do
    ./build_p_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} mi
done

for i in "${tests_my_tests[@]}"
do
    ./build_my_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH}
done

for i in "${tests_my_c_tests[@]}"
do
    ./build_my_test.sh $i mytests/c_env.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} "C"
done

for i in "${tests_compliance[@]}"
do
    ./build_compliance_test.sh $i linker_script.ld rv${ARCH} rv${ARCH}_generated/compiled_files ${ARCH} ui
done

touch rv${ARCH}_generated/done
