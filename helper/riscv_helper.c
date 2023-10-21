#include <stdio.h>
#include <stdarg.h>

#include "riscv_helper.h"

void pass(void)
{
    RISCV_HELPER_DEBUG("pass!\n");
    while(1);
}

void riscv_helper_put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

int riscv_helper_printf (const char *fmt, ...)
{
    char buffer[99], *sptr = buffer;
    va_list va;
    int rslt;
    va_start(va, fmt);
    rslt = vsnprintf(buffer, sizeof(buffer), fmt, va);
    va_end(va);
    while (*sptr) riscv_helper_put_char(*sptr++);
    return rslt;
}
