set -e

root_dir=${PWD}
cd pmp/userspace_test_programs

tests=(
    test_everything_ok
    test_mprv_s_mode
    test_mprv_m_mode
    test_na4_ok
    test_na4_err
    test_tor_ok
    test_tor_err
    test_tor_first_entry
    test_tor_first_entry_err
    test_do_nothing
)

OUTPUT_DIR=build_output

mkdir -p ${OUTPUT_DIR}


# First build "userspace" programs
#cd pmp/userspace_test_programs
./build.sh

# for i in "${tests[@]}"
# do
#     riscv32-none-elf-gcc -march=rv32i -mcmodel=medany -g -Wl,-Ttest.ld,-Bstatic,-Map=output.map -ffreestanding -nostartfiles -specs=nano.specs -o ${OUTPUT_DIR}/${i}_32.elf init.S ${i}.c
#     riscv32-none-elf-objcopy -O binary ${OUTPUT_DIR}/${i}_32.elf ${OUTPUT_DIR}/${i}_32.bin
# done

# for i in "${tests[@]}"
# do
#     riscv64-none-elf-gcc -march=rv64i -mcmodel=medany -g -Wl,-Ttest.ld,-Bstatic,-Map=output.map -ffreestanding -nostartfiles -specs=nano.specs -o ${OUTPUT_DIR}/${i}_64.elf init.S ${i}.c
#     riscv64-none-elf-objcopy -O binary ${OUTPUT_DIR}/${i}_64.elf ${OUTPUT_DIR}/${i}_64.bin
# done
