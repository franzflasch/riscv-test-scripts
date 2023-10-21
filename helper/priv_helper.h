#ifndef PRIV_HELPER_H
#define PRIV_HELPER_H

#include "riscv_helper.h"

// #define PRIV_DEBUG_ENABLE
#ifdef PRIV_DEBUG_ENABLE
#define PRIV_DEBUG(...) do{ riscv_helper_printf( __VA_ARGS__ ); } while( 0 )
#else
#define PRIV_DEBUG(...) do{ } while ( 0 )
#endif

#endif /* PRIV_HELPER_H */
