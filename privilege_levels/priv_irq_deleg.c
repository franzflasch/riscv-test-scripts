#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../helper/priv_helper.h"
#include "../helper/riscv_helper.c"

#define MTVEC_CLIC_VECTORED 1

#define CLINT_MSIP_REG 0x2000000
#define CLINT_TIMCMP_REG 0x02004000
#define CLINT_TIME_REG 0x0200BFF8 

#define UART_TXD_REG 0x10000000

void reload_timer()
{
    volatile rv_uint_xlen * mtime    = (rv_uint_xlen*) (CLINT_TIME_REG);
    volatile rv_uint_xlen * mtimeh   = mtime + 1;
    volatile rv_uint_xlen * mtimecmp = (rv_uint_xlen*) (CLINT_TIMCMP_REG);
    rv_uint_xlen rdtime = 0;

    rdtime = read_csr(time);

    printf("init timer - mtime %lu mtimeh %lu mtimecmp %lu rdtime %lu\n", *mtime, *mtimeh, *mtimecmp, rdtime);

    rv_uint_xlen now = *mtime;
    rv_uint_xlen then = now + 5000000;
    *mtimecmp = then;
}

void supervisor_test_func()
{
    PRIV_DEBUG("supervisor test func!\n");

    /* wait for timer irq */
    while(1);

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

    if(mcause & TRAP_MCAUSE_IRQ)
    {
        PRIV_DEBUG("Timer IRQ!\n");
        reload_timer();
        return;
    }

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
    rv_uint_xlen mie = 0;

    write_csr(mtvec, ((uintptr_t)machine_trap & ~MTVEC_CLIC_VECTORED));

    reload_timer();

    /* Enable Timer interrupt */
    mie = (1<<7);
    write_csr(mie, mie);

    /* supervisor mode */
    mstatus = (1<<TRAP_XSTATUS_MPP_BIT);

    // /* MIE enable */
    // mstatus |= (1<<TRAP_XSTATUS_MIE_BIT);
    // write_csr(mstatus, mstatus);

    /* SIE enable */
    mstatus |= (1<<TRAP_XSTATUS_SIE_BIT);
    write_csr(mstatus, mstatus);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("mstatus %x\n", mstatus);

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = (rv_uint_xlen) supervisor_test_func;
    PRIV_DEBUG("mepc: %x\n", mepc);
    write_csr(mepc, mepc);

    __asm__ volatile("mret");
}
