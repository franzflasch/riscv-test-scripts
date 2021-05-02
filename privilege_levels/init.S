.section .text
.globl _start

_start:
    la sp, _stack
    call kmain
