#ifndef PMP_HELPER_H
#define PMP_HELPER_H

#include "riscv_helper.h"

// #define PMP_DEBUG_ENABLE
#ifdef PMP_DEBUG_ENABLE
#define PMP_DEBUG(...) do{ riscv_helper_printf( __VA_ARGS__ ); } while( 0 )
#else
#define PMP_DEBUG(...) do{ } while ( 0 )
#endif

void __attribute__ ((interrupt)) pmp_interrupt_handler(void);
rv_uint_xlen pmp_get_napot_addr(rv_uint_xlen base, rv_uint_xlen size);
rv_uint_xlen pmp_get_na_tor_addr(rv_uint_xlen base);

#endif /* PMP_HELPER_H */
