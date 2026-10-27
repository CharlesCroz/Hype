.org 0

.text

.globl mess_table

.section .mess_table,"ax"
mess_table:
    push { r0, r1 }

	ldr r0, =MMUTable
    add r0, r0, #0x4
    ldr r1, [r0]
    orr r1, #(0b1 << 15) @ turn on APX
    str r1, [r0]

    /* Set domain permission */
    ldr r1, =0x55555555
    mcr p15, 0, r1, c3, c0, 0

    dsb
    isb
    mcr p15, 0, r0, c8, c7, 0 @ Invalidate TLB cache
    dsb
    isb

    pop { r0, r1 }

    mov pc, lr

.end
