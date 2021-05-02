/* ugly, but this will do for now */
#include "pmp_helper.c"

#if (__riscv_xlen == 64)
    #include "../generated_files/rv64/compiled_files/pmp_user_simple_program_rv64.c"
    #define USER_PROGRAM generated_files_rv64_compiled_files_pmp_user_simple_program_rv64_bin
    #define USER_PROGRAM_LEN generated_files_rv64_compiled_files_pmp_user_simple_program_rv64_bin_len
#else
    #include "../generated_files/rv32/compiled_files/pmp_user_simple_program_rv32.c"
    #define USER_PROGRAM generated_files_rv32_compiled_files_pmp_user_simple_program_rv32_bin
    #define USER_PROGRAM_LEN generated_files_rv32_compiled_files_pmp_user_simple_program_rv32_bin_len
#endif

/* Test description:
   This test will configure the pmp for the memory @0x80000000 and 0x80080000
   but not for the UART. However we will the set to S-Mode bits and also the MPRV bit
   which will apply the settings immediately. As we are in S-mode afterwards
   access to the UART will be denied as soon as we try to access it.
   Expected output on qemu is:
   interrupt handler - mcause: 0000000000000007 mepc: 0000000080000030
 */
int main()
{
    int i = 0;
    rv_uint_xlen pmpcfg0 = 0;
    rv_uint_xlen pmpaddr0 = 0;
    rv_uint_xlen pmpaddr1 = 0;
    rv_uint_xlen pmpaddr2 = 0;

    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mepc = 0;


    rv_uint_xlen phys_addr_to_use = 0x80080000;
    void  *tmp = (rv_uint_xlen *)phys_addr_to_use;
    memcpy(tmp, USER_PROGRAM, USER_PROGRAM_LEN);

    PMP_DEBUG("Hello!\n");

    /* Set interrupt handler, non vectored mode */
    __asm__ volatile ("csrw mtvec, %0" :: "r"((uintptr_t)interrupt_handler & ~MTVEC_CLIC_VECTORED));


    pmpaddr0 = get_pmp_napot_addr(0x80000000, 0x8000);
    write_csr(pmpaddr0, pmpaddr0);

    pmpaddr1 = get_pmp_napot_addr(0x80080000, 0x4000);
    write_csr(pmpaddr1, pmpaddr1);

    /* For UART access */
    pmpaddr2 = get_pmp_napot_addr(UART_TXD_REG, 0x100);
    write_csr(pmpaddr2, pmpaddr2);

    /* Enable NAPOT mode with read write and execute */
    pmpcfg0 = (3 << 3 | 0x7);
    pmpcfg0 |= (3 << 3 | 0x7) << 8;

    /* Do not allow uart access, for testing purposes */
    // pmpcfg0 |= (3 << 3 | 0x7) << 16;
    write_csr(pmpcfg0, pmpcfg0);

    pmpcfg0 = read_csr(pmpcfg0);
    PMP_DEBUG("pmpcfg0 " PRINTF_FMT "\n", pmpcfg0);
    PMP_DEBUG("pmpaddr0 " PRINTF_FMT "\n", pmpaddr0);
    PMP_DEBUG("pmpaddr1 " PRINTF_FMT "\n", pmpaddr1);
    PMP_DEBUG("pmpaddr2 " PRINTF_FMT "\n", pmpaddr2);

    /* Enable supervisor mode and mprv*/
    /* It seems that if MPRV is set, the MPP values are taken immediately, 
       without switching the privilege mode with mret
     */
    mstatus = (1<<17);
    mstatus |= (1<<11);
    write_csr(mstatus, mstatus);

    /* This should throw an exception because uart is not covered in our pmp settings */
    PMP_DEBUG("program end.\n");
    pass();

    return 0;
}
