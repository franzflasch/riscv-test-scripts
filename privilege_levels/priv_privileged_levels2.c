#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "priv_helper.c"

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

    rv_uint_xlen sstatus = (1<<18) | (1<<8);
    write_csr(sstatus, sstatus);

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
    rv_uint_xlen sstatus = 0;
    rv_uint_xlen mcause = 0;
    rv_uint_xlen mepc = 0;

    mcause = read_csr(mcause);
    PRIV_DEBUG("Trap! mcause: %x\n", mcause);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("Trap! mstatus: %x\n", mstatus);

    sstatus = read_csr(sstatus);
    PRIV_DEBUG("Trap! sstatus: %x\n", sstatus);

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
    rv_uint_xlen sstatus = 0;

    write_csr(mtvec, ((uintptr_t)machine_trap & ~MTVEC_CLIC_VECTORED));

    /* start with user mode */
    mstatus = (1<<18) | (0<<11);
    write_csr(mstatus, mstatus);

    sstatus = (1<<18) | (0<<8);
    write_csr(sstatus, sstatus);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("mstatus: %x\n", mstatus);

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = (rv_uint_xlen) user_test_func;
    PRIV_DEBUG("mepc: %x\n", mepc);
    write_csr(mepc, mepc);

    __asm__ volatile("mret");
}
