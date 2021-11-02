riscv32-none-elf-objdump -d simple3.elf | sed -n '/_start/,' | tail -n +2 | nl -v 0
