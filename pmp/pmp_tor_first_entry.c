/* ugly, but this will do for now */
#include "../helper/riscv_helper.c"
#include "../helper/pmp_helper.c"

#if (__riscv_xlen == 64)
    #include "../generated_files/rv64/compiled_files/pmp_user_tor_access_ok_rv64.c"
    #define USER_PROGRAM generated_files_rv64_compiled_files_pmp_user_tor_access_ok_rv64_bin
    #define USER_PROGRAM_LEN generated_files_rv64_compiled_files_pmp_user_tor_access_ok_rv64_bin_len
#else
    #include "../generated_files/rv32/compiled_files/pmp_user_tor_access_ok_rv32.c"
    #define USER_PROGRAM generated_files_rv32_compiled_files_pmp_user_tor_access_ok_rv32_bin
    #define USER_PROGRAM_LEN generated_files_rv32_compiled_files_pmp_user_tor_access_ok_rv32_bin_len
#endif

#define TEST_ACCESS_LOCATION_START 0x80086000
#define TEST_ACCESS_LOCATION_END 0x80087000

/* Test description:
   This test is meant to setup everything the right way
   so the pmp does not throw any error
 */
int main()
{
    int i = 0;
    rv_uint_xlen pmpcfg0 = 0;
    rv_uint_xlen pmpaddr0 = 0;

    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mepc = 0;


    rv_uint_xlen phys_addr_to_use = 0x80080000;
    void  *tmp = (rv_uint_xlen *)phys_addr_to_use;
    memcpy(tmp, USER_PROGRAM, USER_PROGRAM_LEN);

    PMP_DEBUG("Hello!\n");

    /* Set interrupt handler, non vectored mode */
    __asm__ volatile ("csrw mtvec, %0" :: "r"((uintptr_t)pmp_interrupt_handler & ~MTVEC_CLIC_VECTORED));


    pmpaddr0 = pmp_get_na_tor_addr(TEST_ACCESS_LOCATION_END);
    write_csr(pmpaddr0, pmpaddr0);

    /* Enable TOR mode with read write and execute */
    pmpcfg0 = (1 << 3 | 0x7);
    write_csr(pmpcfg0, pmpcfg0);

    pmpcfg0 = read_csr(pmpcfg0);
    PMP_DEBUG("pmpcfg0 " PRINTF_FMT "\n", pmpcfg0);
    PMP_DEBUG("pmpaddr0 " PRINTF_FMT "\n", pmpaddr0);

    /* Enable supervisor mode and mprv*/
    mstatus = (1<<17);
    mstatus |= (1<<11);
    write_csr(mstatus, mstatus);

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = phys_addr_to_use;
    PMP_DEBUG("mepc: %x\n", mepc);

    write_csr(mepc, mepc);

    mstatus = read_csr(mstatus);
    PMP_DEBUG("mstatus: " PRINTF_FMT "\n", mstatus);

     __asm__ volatile("mret");

    PMP_DEBUG("program end.\n");
    pass();

    return 0;
}
