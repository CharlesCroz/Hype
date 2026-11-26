.org 0

.text

.globl hackjump

.section .hackjump_s,"ax"
hackjump:
    push {lr}
    blx r0
    pop {pc}
	

.end
