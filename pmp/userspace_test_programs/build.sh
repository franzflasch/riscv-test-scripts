set -e

tests=(
    test_simple_program
    test_na4_access_ok
    test_na4_access_err
    test_tor_access_ok
    test_tor_access_err
)

POST_FIX="user"

ROOT_DIR="${PWD}"
OUT_DIR="output"
OUT_DIR32="${OUT_DIR}/output32"
OUT_DIR64="${OUT_DIR}/output64"

mkdir -p ${OUT_DIR}
mkdir -p ${OUT_DIR32}
mkdir -p ${OUT_DIR64}

for i in "${tests[@]}"
do
    # 32 Bit
    riscv32-none-elf-gcc -g -march=rv32i -mcmodel=medany -Wl,-Tuser.ld,-Map=${OUT_DIR32}/${i}_${POST_FIX}.map -nostartfiles -o ${OUT_DIR32}/${i}_${POST_FIX}.elf init.S ${i}.c
    riscv32-none-elf-objcopy -O binary ${OUT_DIR32}/${i}_${POST_FIX}.elf ${OUT_DIR32}/${i}_${POST_FIX}.bin

    # print binary as c array
    cd ${OUT_DIR32}
    xxd -i -a ${i}_${POST_FIX}.bin > ${i}_${POST_FIX}.c
    cd ${ROOT_DIR}
done

for i in "${tests[@]}"
do
    # 64 Bit
    riscv64-none-elf-gcc -g -march=rv64i -mcmodel=medany -Wl,-Tuser.ld,-Map=${OUT_DIR64}/${i}_${POST_FIX}.map -nostartfiles -o ${OUT_DIR64}/${i}_${POST_FIX}.elf init.S ${i}.c
    riscv64-none-elf-objcopy -O binary ${OUT_DIR64}/${i}_${POST_FIX}.elf ${OUT_DIR64}/${i}_${POST_FIX}.bin

    # print binary as c array
    cd ${OUT_DIR64}
    xxd -i -a ${i}_${POST_FIX}.bin > ${i}_${POST_FIX}.c
    cd ${ROOT_DIR}
done
