#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define UART_TXD_REG 0x10000000

#if (__riscv_xlen == 64)
typedef uint64_t rv_uint_xlen;
#define PRINTF_FMT "%016lx"
#else
typedef uint32_t rv_uint_xlen;
#define PRINTF_FMT "%08x"
#endif

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

void pass(void)
{
    printf("pass!\n");
    while(1);
}

#define RAM_BASE_ADDR 0x80000000UL
#define RAM_SIZE_BYTES 0x8000000UL 
#define RAM_ADDR_END (RAM_BASE_ADDR + (RAM_SIZE_BYTES-1))

int main()
{
    /* Modify the memsize here to test different read/mem accesses */
    uint32_t *tmp_ptr = (uint32_t *)RAM_ADDR_END;

    printf("RAM end "PRINTF_FMT"\n", RAM_ADDR_END);

    *tmp_ptr = 0x42;

    printf("%x\n", *tmp_ptr);

    pass();
    return 0;
}
