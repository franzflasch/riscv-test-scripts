#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MMU_DEBUG_ENABLE
#ifdef MMU_DEBUG_ENABLE
#define MMU_DEBUG(...) do{ printf( __VA_ARGS__ ); } while( 0 )
#else
#define MMU_DEBUG(...) do{ } while ( 0 )
#endif

#if (__riscv_xlen == 64)
    typedef uint64_t rv_uint_xlen;
    #define PRINTF_FMT "%016lx"
#else
    typedef uint32_t rv_uint_xlen;
    #define PRINTF_FMT "%08x"
#endif

#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })


#define UART_TXD_REG 0x10000000

void put_char(char c)
{
    char *txd = (char *) UART_TXD_REG;
    *txd = c;
}

int printf (const char *fmt, ...)
{
    char buffer[99], *sptr = buffer;
    va_list va;
    int rslt;
    va_start(va, fmt);
    rslt = vsnprintf(buffer, sizeof(buffer), fmt, va);
    va_end(va);
    while (*sptr) put_char(*sptr++);
    return rslt;
}

#define MTVEC_CLIC_VECTORED 1

void pass(void)
{
    MMU_DEBUG("pass!\n");
    while(1);
}
