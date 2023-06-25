#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define MTVEC_CLIC_VECTORED 1

#define UART_TXD_REG 0x10000000

// int global_x = 10;

void put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

void test_invalid_write(char c)
{
    char *txd = (char *) 0x800C0000;
    *txd = c;
}

void test_read(void)
{
    char *txd = (char *) 0x2008;
    char test = *txd;
    put_char(test);
}

// __attribute__((section(".test_mmu_section"))) void test_print(void)
// {
//     int i = 0;

//     for(i=0;i<42;i++)
//     {
//         put_char('Y');
//     }
//     put_char('\n');

// }

void __attribute__ ((interrupt)) supervisor_trap(void)
{
    put_char('S');
    put_char('U');
    put_char('P');
    put_char('E');
    put_char('R');
    put_char('T');
    put_char('R');
    put_char('A');
    put_char('P');
    put_char('\n');
}

int main()
{
    // __asm__ volatile ("csrw stvec, %0" :: "r"((uintptr_t)supervisor_trap & ~MTVEC_CLIC_VECTORED));

    uint32_t sepc = 0;
    uint32_t sstatus = 0;

    put_char('I');
    put_char('N');
    put_char(' ');
    put_char('S');
    put_char('U');
    put_char('P');
    put_char('E');
    put_char('R');
    put_char(' ');
    put_char('M');
    put_char('O');
    put_char('D');
    put_char('E');
    put_char('\n');

    // test_invalid_write(42);
    test_read();

    __asm__ volatile("ecall");

    while(1);

    return 0;
}
