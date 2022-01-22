.section .text
.globl _start

_start:
    jal L1
    nop
L1:
    li ra,0
    mv a4, ra
    li t2,0
    li gp,2
    bne a4,t2, fail
    bne zero,gp,pass

fail:
    jal _start

pass:
    nop
    nop
