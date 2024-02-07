#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "../helper/priv_helper.h"
#include "../helper/pmp_helper.h"

#include "../helper/riscv_helper.c"
#include "../helper/pmp_helper.c"

#define MTVEC_CLIC_VECTORED 1

#define UART_TXD_REG 0x10000000

void user_test_func()
{
    PRIV_DEBUG("user test func!\n");
    __asm__ volatile("ecall");
}

void supervisor_test_func()
{
    PRIV_DEBUG("supervisor test func!\n");
    __asm__ volatile("ecall");
}

void machine_test_func()
{
    PRIV_DEBUG("machine test func!\n");
    __asm__ volatile("ecall");
}

void __attribute__ ((interrupt)) machine_trap(void)
{
    #define MCAUSE_ECALL 0xb

    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mcause = 0;
    rv_uint_xlen mepc = 0;

    mcause = read_csr(mcause);
    PRIV_DEBUG("Trap! mcause: %x\n", mcause);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("Trap! mstatus: %x\n", mstatus);

    if(mcause == 8)
    {
        PRIV_DEBUG("Ecall from user mode\n");

        /* switch to supervisor mode */
        mstatus = (1<<18) | (1<<11);
        write_csr(mstatus, mstatus);

        mepc = (rv_uint_xlen) supervisor_test_func;
        write_csr(mepc, mepc);
        __asm__ volatile("mret");
    }
    else if(mcause == 9)
    {
        PRIV_DEBUG("Ecall from supervisor mode\n");

        mstatus = (1<<18) | (3<<11);
        write_csr(mstatus, mstatus);

        mepc = (rv_uint_xlen) machine_test_func;
        write_csr(mepc, mepc);
        __asm__ volatile("mret");
    }
    else if(mcause == 11)
    {
        PRIV_DEBUG("Ecall from machine mode\n");
        pass();
    }

    while(1);
}

void kmain(void)
{
    rv_uint_xlen mepc = 0;
    rv_uint_xlen mstatus = 0;

    rv_uint_xlen pmpcfg0 = 0;
    rv_uint_xlen pmpaddr0 = 0;
    rv_uint_xlen pmpaddr1 = 0;
    rv_uint_xlen pmpaddr2 = 0;

    write_csr(mtvec, ((uintptr_t)machine_trap & ~MTVEC_CLIC_VECTORED));

    /* since qemu 6.x it is needed to configure the PMP otherwise an illegal instruction gets triggered after
       switching to user or supervisor mode 
       https://stackoverflow.com/questions/69133848/risc-v-illegal-instruction-exception-when-switching-to-supervisor-mode
    */
    pmpaddr0 = pmp_get_napot_addr(0x80000000, 0x8000);
    write_csr(pmpaddr0, pmpaddr0);

    pmpaddr1 = pmp_get_napot_addr(0x80080000, 0x4000);
    write_csr(pmpaddr1, pmpaddr1);

    /* For UART access */
    pmpaddr2 = pmp_get_napot_addr(UART_TXD_REG, 0x100);
    write_csr(pmpaddr2, pmpaddr2);

    /* Enable NAPOT mode with read write and execute */
    pmpcfg0 = (3 << 3 | 0x7);
    pmpcfg0 |= (3 << 3 | 0x7) << 8;
    pmpcfg0 |= (3 << 3 | 0x7) << 16;
    write_csr(pmpcfg0, pmpcfg0);

    pmpcfg0 = read_csr(pmpcfg0);
    PRIV_DEBUG("pmpcfg0 " PRINTF_FMT "\n", pmpcfg0);
    PRIV_DEBUG("pmpaddr0 " PRINTF_FMT "\n", pmpaddr0);
    PRIV_DEBUG("pmpaddr1 " PRINTF_FMT "\n", pmpaddr1);
    PRIV_DEBUG("pmpaddr2 " PRINTF_FMT "\n", pmpaddr2);

    /* start with user mode */
    mstatus = (1<<18) | (0<<11);
    write_csr(mstatus, mstatus);

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = (rv_uint_xlen) user_test_func;
    PRIV_DEBUG("mepc: %x\n", mepc);
    write_csr(mepc, mepc);

    __asm__ volatile("mret");
}
