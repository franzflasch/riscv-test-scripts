#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define UART_TXD_REG 0x10000000

void put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

int main()
{
    int i = 0;
    for(i=0;i<10;i++)
    {
        put_char('X');
        put_char('Y');
        put_char('\n');
    }

    __asm__ volatile("ecall");

    while(1);

    return 0;
}
