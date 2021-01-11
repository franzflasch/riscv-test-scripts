#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MSTATUS_MIE_BIT (1<<3)

#define MIE_MSIP_BIT (1<<3)
#define MIE_MTIP_BIT (1<<7)

#define MTVEC_CLIC_VECTORED 1

#define CLINT_MSIP_REG 0x2000000
#define TEST_REG 0x2000040

#define MCAUSE_MSI 0x3
#define MCAUSE_ECALL 0xb

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

void __attribute__ ((interrupt)) interrupt_handler(void)
{
    uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;
    uint32_t mcause = 0;
    uintptr_t mepc = 0;

    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));

    if(mcause == MCAUSE_ECALL)
    {
        /* Advance mepc by 4 */
        __asm__ volatile("csrr %0, mepc" : "=r"(mepc));
        mepc += 4;
        __asm__ volatile ("csrw mepc, %0" :: "r"(mepc));
    }
    else if(mcause == MCAUSE_MSI)
    {
        /* disable SW interrupt */
        *p_msip = 0;
    }

    // __asm__ volatile ("csrci mie, 0x8");
    // __asm__ volatile ("csrci mip, 0x8");
    // clear_csr(mip, MIE_MSIP_BIT);
    // __asm__ volatile("mret");

    /* disable SW interrupt */
    *p_msip = 0;
}

void pass(void)
{
    __asm__ volatile ("nop");
    __asm__ volatile ("nop");
    return;
}

int main()
{
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t mstatus = 0;
    uint32_t mip = 0;

    uint32_t *p_bla = &x;

    // __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));

    /* Set interrupt handler, non vectored mode */
    __asm__ volatile ("csrw mtvec, %0" :: "r"((uintptr_t)interrupt_handler & ~MTVEC_CLIC_VECTORED));

    set_csr(mstatus, MSTATUS_MIE_BIT);
    set_csr(mie, MIE_MSIP_BIT);

    // __asm__ volatile("csrr %0, mip" : "=r"(mip));
    // put_char('l');

    // *p_msip = 0;
    // __asm__ volatile("csrr %0, mip" : "=r"(mip));
    // put_char('l');

    /* trigger ecall */
    __asm__ volatile("ecall");

    while(1)
    {
        x++;
        if(x > 10)
        {
            y += x;
            break;
        }
    }

    pass();

    return 0;
}
