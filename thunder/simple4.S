.section .text
.globl _start

_start:
        li      a5,42
.L2:
        addi    a5,a5,-1
        bne     a5,zero,.L2
        li      a0,42
        ret