#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// #define PMP_DEBUG_ENABLE
#ifdef PMP_DEBUG_ENABLE
#define PMP_DEBUG(...) do{ printf( __VA_ARGS__ ); } while( 0 )
#else
#define PMP_DEBUG(...) do{ } while ( 0 )
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
    PMP_DEBUG("pass!\n");
    while(1);
}

void __attribute__ ((interrupt)) interrupt_handler(void)
{
    rv_uint_xlen mcause = read_csr(mcause);
    rv_uint_xlen mepc = read_csr(mepc);

    PMP_DEBUG("interrupt handler - mcause: "PRINTF_FMT" mepc: " PRINTF_FMT"\n", mcause, mepc);

    pass();
}

rv_uint_xlen get_pmp_napot_addr(rv_uint_xlen base, rv_uint_xlen size) {
    rv_uint_xlen napot_size = ((size/2)-1);
    //napot_size = 0x7FFF
    rv_uint_xlen pmp_addr= (base + napot_size)>>2;
    //pmp_addr = 0x1000_01FF

    PMP_DEBUG("pmp addr: " PRINTF_FMT "\n", pmp_addr);

    return pmp_addr;
}

rv_uint_xlen get_pmp_na_tor_addr(rv_uint_xlen base) {
    rv_uint_xlen pmp_addr= base>>2;
    //pmp_addr = 0x1000_01FF

    PMP_DEBUG("pmp addr: " PRINTF_FMT "\n", pmp_addr);

    return pmp_addr;
}
