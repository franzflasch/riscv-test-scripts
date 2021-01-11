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
    return;
}


int sum(int n) {
    printf("%d\n", n);
    if (n != 0)
        // sum() function calls itself
        return n + sum(n-1); 
    else
        return n;
}

rv_uint_xlen test = 44;

int main()
{
    int i = 0;
    float x = 0.1;

    printf("Start!! %d\n", test);

    for(i=0;i<100;i++)
    {
        x += (i/100)+(i*x);
    }

    sum(800);

    printf("finish!\n");
    while(1)

    pass();

    return 0;
}
