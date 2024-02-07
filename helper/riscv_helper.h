#ifndef RISCV_CSR_H
#define RISCV_CSR_H

#include <stdint.h>

// #define RISCV_HELPER_DEBUG_ENABLE
#ifdef RISCV_HELPER_DEBUG_ENABLE
#define RISCV_HELPER_DEBUG(...) do{ riscv_helper_printf( __VA_ARGS__ ); } while( 0 )
#else
#define RISCV_HELPER_DEBUG(...) do{ } while ( 0 )
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

#define MTVEC_CLIC_VECTORED 1

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

int riscv_helper_printf (const char *fmt, ...);
void pass(void);

#endif /* RISCV_CSR_H */
