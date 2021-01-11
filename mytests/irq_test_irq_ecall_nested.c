#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MSTATUS_MIE_BIT (1<<3)

#define MIE_MSIP_BIT (1<<3)
#define MIE_MTIP_BIT (1<<7)

#define MTVEC_CLIC_VECTORED 1

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define CLINT_MSIP_REG 0x2000000
#define TEST_REG 0x2000040


#define UART_TXD_REG 0x10000000

#if (__riscv_xlen == 64)
typedef uint64_t rv_uint_xlen;
#define PRINTF_FMT "%016lx"
#else
typedef uint32_t rv_uint_xlen;
#define PRINTF_FMT "%08x"
#endif

uint64_t *p_msip = (uint64_t *)CLINT_MSIP_REG;

void pass(void)
{
    return;
}

#define MCAUSE_MSI ((1UL << (__riscv_xlen-1)) | 0x3)
#define MCAUSE_ECALL 0xb

void __attribute__ ((interrupt)) interrupt_handler(void)
{
    uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;
    rv_uint_xlen mcause = 0;
    uintptr_t mepc = 0;
    uintptr_t mtval = 0;
    rv_uint_xlen mip = 0;

    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    __asm__ volatile("csrr %0, mepc" : "=r"(mepc));
    __asm__ volatile("csrr %0, mtval" : "=r"(mtval));

    if(mcause == MCAUSE_ECALL)
    {
        pass();
        /* Advance mepc by 4 */
        __asm__ volatile("csrr %0, mepc" : "=r"(mepc));
        mepc += 4;
        __asm__ volatile ("csrw mepc, %0" :: "r"(mepc));
    }
    else if(mcause == MCAUSE_MSI)
    {
        __asm__ volatile("csrr %0, mip" : "=r"(mip));

        __asm__ volatile("ecall");

        /* disable SW interrupt */
        *p_msip = 0;

        __asm__ volatile("csrr %0, mip" : "=r"(mip));

        // while(1);
    }
}

int main()
{
    rv_uint_xlen x = 0;
    rv_uint_xlen y = 0;
    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mip = 0;

    uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;
    rv_uint_xlen *p_bla = &x;

    // __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));

    *p_bla = 42;

    /* Set interrupt handler, non vectored mode */
    __asm__ volatile ("csrw mtvec, %0" :: "r"((uintptr_t)interrupt_handler & ~MTVEC_CLIC_VECTORED));

    set_csr(mstatus, MSTATUS_MIE_BIT);
    set_csr(mie, MIE_MSIP_BIT);

    *p_msip = 1;

    __asm__ volatile("csrr %0, mip" : "=r"(mip));

    // __asm__ volatile("csrr %0, mip" : "=r"(mip));
    // put_char('l');

    // *p_msip = 0;
    // __asm__ volatile("csrr %0, mip" : "=r"(mip));
    // put_char('l');

    /* trigger ecall */
    // __asm__ volatile("ecall");

    pass();

    while(1)
    {
        x++;
        if(x==100000)
        {
            y += x;
        }
    }

    return 0;
}
