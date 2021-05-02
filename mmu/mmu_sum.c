#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "mmu_helper.c"

#if (__riscv_xlen == 64)
    #include "../generated_files/rv64/compiled_files/mmu_supervisor_simple_rv64.c"
    #include "../generated_files/rv64/compiled_files/mmu_user_simple_rv64.c"

    #define SUPERVISOR_PROGRAM generated_files_rv64_compiled_files_mmu_supervisor_simple_rv64_bin
    #define SUPERVISOR_PROGRAM_LEN generated_files_rv64_compiled_files_mmu_supervisor_simple_rv64_bin_len

    #define USER_PROGRAM generated_files_rv64_compiled_files_mmu_user_simple_rv64_bin
    #define USER_PROGRAM_LEN generated_files_rv64_compiled_files_mmu_user_simple_rv64_bin_len
#else
    #include "../generated_files/rv32/compiled_files/mmu_supervisor_simple_rv32.c"
    #include "../generated_files/rv32/compiled_files/mmu_user_simple_rv32.c"

    #define SUPERVISOR_PROGRAM generated_files_rv32_compiled_files_mmu_supervisor_simple_rv32_bin
    #define SUPERVISOR_PROGRAM_LEN generated_files_rv32_compiled_files_mmu_supervisor_simple_rv32_bin_len

    #define USER_PROGRAM generated_files_rv32_compiled_files_mmu_user_simple_rv32_bin
    #define USER_PROGRAM_LEN generated_files_rv32_compiled_files_mmu_user_simple_rv32_bin_len
#endif


#define MTVEC_CLIC_VECTORED 1

#define UART_TXD_REG 0x10000000

#define SV32_LEVELS 2
#define SV32_PTESIZE 4
#define SV32_PAGE_TABLE_ENTRIES 1024


#define PAGE_VALID (1<<0)
#define PAGE_READ  (1<<1)
#define PAGE_WRITE (1<<2)
#define PAGE_EXEC  (1<<3)
#define PAGE_USER  (1<<4)
#define PAGE_GLOB  (1<<5)

uint32_t  __attribute__ ((aligned (4096))) page_table[SV32_LEVELS][SV32_PAGE_TABLE_ENTRIES];

uint32_t Sv32_mmu_map_virt(uint32_t virt_addr, uint32_t phys_addr, uint8_t user_page)
{
    uint32_t vpn[SV32_LEVELS] = 
    {
        (virt_addr >> 12) & 0x3ff,
        (virt_addr >> 22) & 0xfff
    };
    MMU_DEBUG("vpn[1] %x vpn[0] %x\n", vpn[1], vpn[0]);

    /* physical addresses are 34 Bit wide!!! even on RV32 systems */
    uint32_t ppn[SV32_LEVELS] = 
    {
        (phys_addr >> 12) & 0x3ff,
        (phys_addr >> 22) & 0xfff
    };
    // MMU_DEBUG("ppn[1] %lx ppn[0] %lx\n", ppn[1], ppn[0]);

    /* set address to the root pagetable + vpn[1] offset */
    uint32_t *pte = &page_table[0][vpn[1]];
    // MMU_DEBUG("pte addr: %p val %lx\n", pte, *pte);

    /* For now we only use 4KiB pages so first level is always a pointer to the next page table */
    *pte = (uint32_t)&page_table[1][0];
    /* shift address by 2 and set valid flag */
    *pte = (*pte >> 2) | PAGE_VALID;

    /* Now set address to the next level (SV32 only has 2 levels) */
    pte = &page_table[1][vpn[0]];

    /* Set physical address here */
    if(user_page)
        *pte = (ppn[1] << 20) | (ppn[0] << 10) | PAGE_USER | PAGE_EXEC | PAGE_READ | PAGE_WRITE | PAGE_VALID;
    else
        *pte = (ppn[1] << 20) | (ppn[0] << 10) | PAGE_EXEC | PAGE_READ | PAGE_WRITE | PAGE_VALID;
}

void print_page_table(void)
{
    int i = 0;
    for(i=0;i<(SV32_PAGE_TABLE_ENTRIES);i++)
    {
        if(page_table[0][i] != 0)
            MMU_DEBUG("PTE1[%d]: %x\n", i, page_table[0][i]);
    }

    for(i=0;i<(SV32_PAGE_TABLE_ENTRIES);i++)
    {
        if(page_table[1][i] != 0)
            MMU_DEBUG("PTE0[%d]: %x\n", i, page_table[1][i]);
    } 
}

void __attribute__ ((interrupt)) machine_trap(void)
{
    #define MCAUSE_ECALL 0xb

    rv_uint_xlen mstatus = 0;
    rv_uint_xlen mcause = 0;
    rv_uint_xlen mepc = 0;

    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    MMU_DEBUG("Trap! mcause: %x\n", mcause);

    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    MMU_DEBUG("Trap! mstatus: %x\n", mstatus);

    __asm__ volatile("csrr %0, mepc" : "=r"(mepc));
    MMU_DEBUG("Trap! mepc: %x\n", mepc);

    if(mcause == 8)
        MMU_DEBUG("Ecall from user mode\n");
    else if(mcause == 9)
        MMU_DEBUG("Ecall from supervisor mode\n");
    else if(mcause == 11)
        MMU_DEBUG("Ecall from machine mode\n");

    while(1);
}

void kmain(void)
{
    rv_uint_xlen i = 0;
    rv_uint_xlen mepc = 0;
    rv_uint_xlen mstatus = 0;
    rv_uint_xlen satp_addr = 0;
    rv_uint_xlen satp = 0;
    rv_uint_xlen supervisor_phys_addr = 0x80080000;
    rv_uint_xlen user_phys_addr = 0x80090000;

    __asm__ volatile ("csrw mtvec, %0" :: "r"((uintptr_t)machine_trap & ~MTVEC_CLIC_VECTORED));

    /* This should actually not be done, this is only here so that userspace and supervisor space can access the same
     * mem area as machine mode. because otherwise we need to add logic for context saving and restoring before switching 
     * privilege modes
     */
    for(i=0x00;i<0x80;i++)
    {
        Sv32_mmu_map_virt(0x80000000+(i*0x1000), 0x80000000+(i*0x1000), 1);
    }

    /* It seems with SUM=1 the supervisor is indeed allowed to access user pages, however it
     * seems it is not allowed to directly run from it. So we need to disable the user flag
     * for the supervisor page
     */
    Sv32_mmu_map_virt(0x2000, supervisor_phys_addr, 1);
    Sv32_mmu_map_virt(0xC000, user_phys_addr, 1);
    Sv32_mmu_map_virt(UART_TXD_REG, UART_TXD_REG, 1);

    print_page_table();

    void  *tmp = (rv_uint_xlen *)supervisor_phys_addr;
    rv_uint_xlen *program_ptr = tmp;
    memcpy(tmp, SUPERVISOR_PROGRAM, SUPERVISOR_PROGRAM_LEN);

    tmp = (rv_uint_xlen *)user_phys_addr;
    program_ptr = tmp;
    memcpy(tmp, USER_PROGRAM, USER_PROGRAM_LEN);

    /* set satp address */
    satp_addr = (rv_uint_xlen)&page_table[0][0];
    MMU_DEBUG("satp addr %x\n", satp_addr);

    /* store it in the satp register */
    satp = (1<<31) | (satp_addr >> 12);
    __asm__ volatile ("csrw satp, %0" :: "r"(satp));

    /* Read back for checking */
    __asm__ volatile("csrr %0, satp" : "=r"(satp));
    MMU_DEBUG("satp reg %x %p\n", satp, &page_table[0][0]);

    /* don't know if that's needed, but i think it doesnt hurt either, if we change satp */
    __asm__ volatile ("sfence.vma zero, %0" :: "r"(0));

    /* If SUM=0 access to user pages from supervisor is not permitted */
    mstatus = (0<<18) | (1<<11);
    __asm__ volatile ("csrw mstatus, %0" :: "r"(mstatus));

    /* set mepc to sub program, this should run from the mmu after mret */
    mepc = 0x2000;
    MMU_DEBUG("mepc: %x\n", mepc);
    __asm__ volatile ("csrw mepc, %0" :: "r"(mepc));

    __asm__ volatile("mret");

    // while(1);
}
