#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define UART_TXD_REG 0x10000000

void put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

#define TEST_ACCESS_LOCATION_START 0x80086000
#define TEST_ACCESS_LOCATION_END 0x80087000

int main()
{
    uint8_t *tmp_ptr_low = (uint8_t *)TEST_ACCESS_LOCATION_START;
    uint8_t *tmp_ptr_high = (uint8_t *)(TEST_ACCESS_LOCATION_END+1);
    int i = 0;

    for(i=0;i<10;i++)
    {
        put_char('X');
        put_char('Y');
        put_char('\n');
    }

    *tmp_ptr_low = i;
    *tmp_ptr_high = i;

    __asm__ volatile("ecall");

    while(1);

    return 0;
}
