#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define MSTATUS_MIE_BIT (1<<3)

#define MIE_MSIP_BIT (1<<3)
#define MIE_MTIP_BIT (1<<7)

#define CLINT_MSIP_REG 0x2000000

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

    uint32_t *p_msip = (uint32_t *)CLINT_MSIP_REG;
    uint32_t *p_bla = &x;

    *p_bla = 42;
    *p_msip = 1;

    // while(1)
    // {
    //     x++;
    //     if(x==100000)
    //     {
    //         y += x;
    //     }
    // }

    pass();

    return 0;
}
