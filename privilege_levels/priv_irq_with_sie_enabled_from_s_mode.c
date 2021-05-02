#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "priv_helper.c"

#define MTVEC_CLIC_VECTORED 1

#define CLINT_MSIP_REG 0x2000000
#define CLINT_TIMCMP_REG 0x02004000
#define CLINT_TIME_REG 0x0200BFF8 

#define UART_TXD_REG 0x10000000

void reload_timer()
{
    volatile rv_uint_xlen * mtime    = (rv_uint_xlen*) (CLINT_TIME_REG);
    volatile rv_uint_xlen * mtimecmp = (rv_uint_xlen*) (CLINT_TIMCMP_REG);

    printf("init timer - mtime %lu mtimecmp %lu\n", *mtime, *mtimecmp);

    rv_uint_xlen now = *mtime;
    rv_uint_xlen then = now + 10000000;
    *mtimecmp = then;
}

void supervisor_test_func()
{
    rv_uint_xlen mip = 0;
    rv_uint_xlen sip = 0;
    rv_uint_xlen sie = 0;

    PRIV_DEBUG("supervisor test func!\n");

    // /* SET MTI pending */
    // mip = (1<<5);
    // write_csr(mip, mip);
    // sip = read_csr(mip);
    // PRIV_DEBUG("supervisor_test_func! sip: %x\n", mip);

    // /* SET STI pending */
    // sie = (1<<5);
    // write_csr(sie, sie);
    // sie = read_csr(sie);
    // PRIV_DEBUG("supervisor_test_func! sip: %x\n", sie);


    /* wait for timer irq */
    while(1);

    __asm__ volatile("ecall");
}

void machine_test_func()
{
    PRIV_DEBUG("machine test func!\n");

    __asm__ volatile("ecall");
}

void __attribute__ ((interrupt)) supervisor_trap(void)
{
    rv_uint_xlen scause = 0;
    rv_uint_xlen sip = 0;

    PRIV_DEBUG("supervisor trap!\n");

    scause = read_csr(scause);
    PRIV_DEBUG("Trap! scause: %x\n", scause);

    sip = read_csr(sip);
    PRIV_DEBUG("Trap! sip: %x\n", sip);

    while(1);
}

void __attribute__ ((interrupt)) machine_trap(void)
{
    rv_uint_xlen mcause = 0;
    rv_uint_xlen mip = 0;
    rv_uint_xlen sip = 0;

    PRIV_DEBUG("machine trap!\n");

    mcause = read_csr(mcause);
    PRIV_DEBUG("Trap! mcause: %x\n", mcause);

    mip = read_csr(mip);
    PRIV_DEBUG("Trap! mip: %x\n", mip);

    sip = read_csr(sip);
    PRIV_DEBUG("Trap! sip: %x\n", sip);

    while(1);
}

// void __attribute__ ((interrupt)) machine_trap(void)
// {
//     #define MCAUSE_ECALL 0xb

//     rv_uint_xlen mstatus = 0;
//     rv_uint_xlen mcause = 0;
//     rv_uint_xlen mepc = 0;

//     PRIV_DEBUG("machine trap!\n");

//     mcause = read_csr(mcause);
//     PRIV_DEBUG("Trap! mcause: %x\n", mcause);

//     mstatus = read_csr(mstatus);
//     PRIV_DEBUG("Trap! mstatus: %x\n", mstatus);

//     if(mcause & TRAP_MCAUSE_IRQ)
//     {
//         PRIV_DEBUG("Timer IRQ!\n");
//         reload_timer();
//         return;
//     }

//     if(mcause == 8)
//     {
//         PRIV_DEBUG("Ecall from user mode\n");

//         /* switch to supervisor mode */
//         mstatus = (1<<18) | (1<<11);
//         write_csr(mstatus, mstatus);

//         mepc = (rv_uint_xlen) supervisor_test_func;
//         write_csr(mepc, mepc);
//         __asm__ volatile("mret");
//     }
//     else if(mcause == 9)
//     {
//         PRIV_DEBUG("Ecall from supervisor mode\n");

//         mstatus = (1<<18) | (3<<11);
//         write_csr(mstatus, mstatus);

//         mepc = (rv_uint_xlen) machine_test_func;
//         write_csr(mepc, mepc);
//         __asm__ volatile("mret");
//     }
//     else if(mcause == 11)
//     {
//         PRIV_DEBUG("Ecall from machine mode\n");
//         pass();
//     }

//     while(1);
// }

void kmain(void)
{
    rv_uint_xlen mepc = 0;
    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mie = 0;
    rv_uint_xlen mip = 0;
    rv_uint_xlen mideleg = 0;

    write_csr(mtvec, ((uintptr_t)machine_trap & ~MTVEC_CLIC_VECTORED));
    write_csr(stvec, ((uintptr_t)supervisor_trap & ~MTVEC_CLIC_VECTORED));

    /* ENABLE STI */
    mie = (1<<5);
    write_csr(mie, mie);
    mie = read_csr(mie);
    PRIV_DEBUG("mie %x\n", mie);

    /* SET MTI pending */
    mip = (1<<5);
    write_csr(mip, mip);
    mip = read_csr(mip);
    PRIV_DEBUG("mip: %x\n", mip);

    /* Enable SIE */
    mstatus |= (1<<TRAP_XSTATUS_SIE_BIT);
    /* Also shift by 4 to xPIE to be sure it is definitely enabled after mret */
    mstatus |= (1<<TRAP_XSTATUS_SIE_BIT) << 4;

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = (rv_uint_xlen) supervisor_test_func;
    PRIV_DEBUG("mepc: %x\n", mepc);
    write_csr(mepc, mepc);

    /* machine mode */
    mstatus |= (1<<TRAP_XSTATUS_MPP_BIT);
    write_csr(mstatus, mstatus);
    mstatus = read_csr(mstatus);
    PRIV_DEBUG("mstatus %x\n", mstatus);
    // mie = read_csr(mie);
    // PRIV_DEBUG("mie %x\n", mie);
    // mideleg = read_csr(mideleg);
    // PRIV_DEBUG("mideleg %x\n", mideleg);

    __asm__ volatile("mret");
}
