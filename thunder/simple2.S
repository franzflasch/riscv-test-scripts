.section .text
.globl _start

_start:
    sub x0, x1, x1
    sub x0, x2, x3
    sub x4, x2, x3
L1:
    sub x5, x2, x3
    sub x7, x5, x5
    sub x7, x6, x4
    jal L1

    sub x5, x2, x3
    sub x7, x5, x5
    sub x7, x6, x4
    jr  x7

    sub x7, x5, x5
    sub x7, x5, x5
    sub x7, x5, x5

