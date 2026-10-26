.org 0

.text

.globl _boot

.section .boot,"ax"
_boot:
@ Todo : Configure FIQ/IRQ stack, some day

@ SCTLR : Disable MMU (bit 0), data cache (bit 2), instruction cache (bit 12)  
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #1 
	bic	r0, r0, #(0x1 << 2)
	bic	r0, r0, #(0x1 << 12)
	mcr	p15, 0, r0, c1, c0, 0

@ Invalidate Instruction Cache to PoU (ICIALLU)
	eor	r0, r0
	mcr p15, 0, r0, c7, c5, 0

@ Loops to invalidate data caches DCISW[31:30]:way(0-3), DCISW[29:5]:set(0,number-1) 
@ Fetch data cache size ID CCSIDR
	mrc	p15, 1, r0, c0, c0, 0
	mov r1, #0x1ff
	and r3, r1, r0, LSR #13 @ r3 is numbers of sets - 1	
	eor r0, r0 @ set index = 0
_boot_invalidate_cache_setloop:
	eor r1, r1 @ way index = 0
_boot_invalidate_cache_wayloop:
	mov r2, r1, LSL #30			@ set r2~DCISW[31:30]:way
	orr r2, r2, r0, LSL #5		@ set r2~DCISW[29:5]:set
	mcr p15, 0, r2, c7, c6, 2 	@ write DCISW to invalidate way/set data cache
	add r1, r1, #1 				@ increment way index 
	cmp r1, #4					@ if <4 loop
	blt _boot_invalidate_cache_wayloop
	add r0, r0, #1				@ increment set index
	cmp r0, r3					@ if <= num-1, loop
	ble _boot_invalidate_cache_setloop

@ Invalidate TLB cache : write anything to TLBIALL
	mcr p15, 0, r0, c8, c7, 0

@ Enable branch prediction @ SCTLR bit 11
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #(0x1 << 11)
	mcr	p15, 0, r0, c1, c0, 0

@ Enable Data prefetch SCTLR bit 2
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #(0x1 << 2)
	mcr	p15, 0, r0, c1, c0, 1

@ Wait
	dsb
	isb

@ Init MMU
@ Set TTBCR to 0, TTBR0 to xx and TTBR1 to yy, DACR to 1s
	eor r0, r0
	mcr p15, 0, r0, c2, c0, 2	@ Write TTBCR
	ldr r0, =MMUTable			@ MMUTable address
	orr r0, r0, #0x5b 			@ Outer-cacheable, WB
	mcr p15, 0, r0, c2, c0, 0	@ Write TTBR0
	mvn	r0,#0					@ All ones
	mcr	p15,0,r0,c3,c0,0		@ Write DACR

@ Enable MMU
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #1 
	mcr	p15, 0, r0, c1, c0, 0

@ Set main return address to _exit
	ldr sp, =_stack
	ldr lr, =_exit
	b main
	

.end
