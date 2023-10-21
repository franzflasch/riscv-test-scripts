#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../helper/priv_helper.h"
#include "../helper/pmp_helper.h"

#include "../helper/riscv_helper.c"
#include "../helper/pmp_helper.c"

#define MTVEC_CLIC_VECTORED 1

#define UART_TXD_REG 0x10000000

#define MSTATUS_MPIE_BIT ((1<<3) << 4)
#define MSTATUS_SPIE_BIT ((1<<1) << 4)

#define MSTATUS_MIE_BIT (1<<3)
#define MSTATUS_SIE_BIT (1<<1)

#define MIE_MSIP_BIT (1<<3)
#define SIE_SSIP_BIT (1<<1)
#define MIE_MTIP_BIT (1<<7)

#define CLINT_MSIP_REG 0x2000000

void user_test_func()
{
    PRIV_DEBUG("user test func!\n");
    __asm__ volatile("ecall");
}

void supervisor_test_func()
{
    PRIV_DEBUG("supervisor test func!\n");

    // rv_uint_xlen sstatus = read_csr(sstatus);

    // sstatus |= (1<<8);
    // write_csr(sstatus, sstatus);

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

    PRIV_DEBUG("machine trap!\n");

    mepc = read_csr(mepc);
    PRIV_DEBUG("Trap! mepc: "PRINTF_FMT"\n", mepc);

    mcause = read_csr(mcause);
    PRIV_DEBUG("Trap! mcause: "PRINTF_FMT"\n", mcause);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("Trap! mstatus: "PRINTF_FMT"\n", mstatus);

    sstatus = read_csr(sstatus);
    PRIV_DEBUG("Trap! sstatus: "PRINTF_FMT"\n", sstatus);

    if( (mcause & 0xFF) == 3)
    {
        PRIV_DEBUG("MACHINE SW IRQ!\n");

        /* clear msi */
        uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;
        *p_msip = 0;

        __asm__ volatile("mret");
    }

    if(mcause == 8)
    {
        PRIV_DEBUG("Ecall from user mode\n");

        /* switch to supervisor mode */
        mstatus = (1<<11);
        write_csr(mstatus, mstatus);

        mepc = (rv_uint_xlen) supervisor_test_func;
        write_csr(mepc, mepc);
        __asm__ volatile("mret");
    }
    else if(mcause == 9)
    {
        PRIV_DEBUG("Ecall from supervisor mode\n");

        mstatus = (3<<11);
        write_csr(mstatus, mstatus);

        mepc = (rv_uint_xlen) machine_test_func;
        write_csr(mepc, mepc);
        __asm__ volatile("mret");
    }
    else if(mcause == 11)
    {
        PRIV_DEBUG("Ecall from machine mode\n");
    }

    while(1);
}

void __attribute__ ((interrupt)) supervisor_trap(void)
{
    PRIV_DEBUG("supervisor trap!\n");

    rv_uint_xlen mstatus = 0;
    rv_uint_xlen sie = 0;
    rv_uint_xlen sip = 0;
    rv_uint_xlen sepc = 0;
    rv_uint_xlen scause = 0;
    rv_uint_xlen sstatus = 0;
    uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;

    sstatus = read_csr(sstatus);
    PRIV_DEBUG("Trap! sstatus: "PRINTF_FMT"\n", sstatus);

    scause = read_csr(scause);
    PRIV_DEBUG("Trap! scause: "PRINTF_FMT"\n", scause);

    sie = read_csr(sie);
    PRIV_DEBUG("Trap! sie: "PRINTF_FMT"\n", sie);

    /* trigger SW interrupt here, this should immediately trap to the machine trap handler */
    *p_msip = 1;

    sip = read_csr(sip);
    PRIV_DEBUG("Trap! sip: "PRINTF_FMT"\n", sip);

    if(scause == 8)
    {
        PRIV_DEBUG("Ecall from user mode\n");

        /* switch to supervisor mode */
        sstatus = (1<<8);
        write_csr(sstatus, sstatus);

        sepc = (rv_uint_xlen) supervisor_test_func;
        write_csr(sepc, sepc);
        __asm__ volatile("sret");
    }
    else if(scause == 9)
    {
        PRIV_DEBUG("Ecall from supervisor mode\n");    
    }

    pass();
}

void kmain(void)
{
    rv_uint_xlen mepc = 0;
    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mie = 0;
    rv_uint_xlen medeleg = 0;
    rv_uint_xlen sstatus = 0;

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

    /* start with user mode and enable MIE */
    mstatus = MSTATUS_MPIE_BIT | MSTATUS_SPIE_BIT | MSTATUS_MIE_BIT | MSTATUS_SIE_BIT;
    write_csr(mstatus, mstatus);

    write_csr(stvec, ((uintptr_t)supervisor_trap & ~MTVEC_CLIC_VECTORED));

    /* Delegate supervisor ecall and user ecall to supervisor trap */
    medeleg = (1<<9) | (1<<8);
    write_csr(medeleg, medeleg);

    // sstatus = (0<<8);
    // write_csr(sstatus, sstatus);

    mie = MIE_MSIP_BIT | SIE_SSIP_BIT;
    write_csr(mie, mie);

    mstatus = read_csr(mstatus);
    PRIV_DEBUG("mstatus: %x\n", mstatus);

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = (rv_uint_xlen) user_test_func;
    PRIV_DEBUG("mepc: %x\n", mepc);
    write_csr(mepc, mepc);

    __asm__ volatile("mret");
}
