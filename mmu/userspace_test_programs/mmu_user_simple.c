#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define UART_TXD_REG 0x10000000

// int global_x = 10;

void put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

// void test_invalid_write(char c)
// {
//     char *txd = (char *) 0x80090000;
//     *txd = c;
// }

// __attribute__((section(".test_mmu_section"))) void test_print(void)
// {
//     int i = 0;

//     for(i=0;i<42;i++)
//     {
//         put_char('Y');
//     }
//     put_char('\n');

// }

int main()
{
    put_char('I');
    put_char('N');
    put_char(' ');
    put_char('U');
    put_char('S');
    put_char('E');
    put_char('R');
    put_char(' ');
    put_char('M');
    put_char('O');
    put_char('D');
    put_char('E');
    put_char('\n');

    __asm__ volatile("ecall");

    while(1);

    return 0;
}
