#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PRIV_DEBUG_ENABLE
#ifdef PRIV_DEBUG_ENABLE
#define PRIV_DEBUG(...) do{ printf( __VA_ARGS__ ); } while( 0 )
#else
#define PRIV_DEBUG(...) do{ } while ( 0 )
#endif

#if (__riscv_xlen == 64)
    typedef uint64_t rv_uint_xlen;
    #define PRINTF_FMT "%016lx"
    #define TRAP_MCAUSE_IRQ (1<< (64))
#else
    typedef uint32_t rv_uint_xlen;
    #define PRINTF_FMT "%08x"
    #define TRAP_MCAUSE_IRQ (1<< (31))
#endif

#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define TRAP_XSTATUS_SIE_BIT 1
#define TRAP_XSTATUS_MIE_BIT 3
#define TRAP_XSTATUS_UPIE_BIT 4
#define TRAP_XSTATUS_SPIE_BIT 5
#define TRAP_XSTATUS_MPIE_BIT 7
#define TRAP_XSTATUS_SPP_BIT 8
#define TRAP_XSTATUS_MPP_BIT 11 /* and 12 */
#define TRAP_XSTATUS_MPRV_BIT 17
#define TRAP_XSTATUS_SUM_BIT 18
#define TRAP_XSTATUS_MXR_BIT 19

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
    PRIV_DEBUG("pass!\n");
    while(1);
}
