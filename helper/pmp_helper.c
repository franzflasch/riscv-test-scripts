#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "riscv_helper.h"
#include "pmp_helper.h"

void __attribute__ ((interrupt)) pmp_interrupt_handler(void)
{
    rv_uint_xlen mcause = read_csr(mcause);
    rv_uint_xlen mepc = read_csr(mepc);
    rv_uint_xlen mstatus = read_csr(mstatus);

    PMP_DEBUG("interrupt handler - mcause: "PRINTF_FMT" mepc: " PRINTF_FMT" mstatus: " PRINTF_FMT"\n", mcause, mepc, mstatus);

    pass();
}

rv_uint_xlen pmp_get_napot_addr(rv_uint_xlen base, rv_uint_xlen size)
{
    rv_uint_xlen napot_size = ((size/2)-1);
    //napot_size = 0x7FFF
    rv_uint_xlen pmp_addr= (base + napot_size)>>2;
    //pmp_addr = 0x1000_01FF

    PMP_DEBUG("pmp addr: " PRINTF_FMT "\n", pmp_addr);

    return pmp_addr;
}

rv_uint_xlen pmp_get_na_tor_addr(rv_uint_xlen base) 
{
    rv_uint_xlen pmp_addr= base>>2;
    //pmp_addr = 0x1000_01FF

    PMP_DEBUG("pmp addr: " PRINTF_FMT "\n", pmp_addr);

    return pmp_addr;
}
