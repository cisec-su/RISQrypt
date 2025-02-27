.globl _start
_start:
li t1, 0x10008014
lw t2, 0(t1)
beqz t2, 0x1000
j 0x20