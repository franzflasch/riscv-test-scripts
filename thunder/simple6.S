.section .text
.globl _start

_start:
        addi    a5,a5,-1
        addi    a5,a5,-1
        addi    a5,a5,-1
        addi    a5,a5,-1
        addi    a5,a5,-1
        addi    a5,a5,-1
        bne     a5,zero,.l1
        addi    a5,a5,-1
        j       .l2

.l1:
        addi    a5,a5,-1
.l2:
        addi    a5,a5,-1
        addi    a5,a5,-1
        addi    a5,a5,-1

        ret
