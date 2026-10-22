.org 0

.text

.globl _vector_table

.section .vectors
_vector_table:
	B	_boot
	B	Undefined
	B	SVCHandler
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	/* Placeholder for address exception vector*/
	B	IRQHandler
/*	B	FIQHandler Undefining this so the exception directly points to FIQ handler code */

FIQHandler:			            /* FIQ vector handler */
	subs	pc, lr, #4			/* adjust return */

IRQHandler:					    /* IRQ vector handler */
	subs	pc, lr, #4			/* adjust return */

Undefined:					/* Undefined handler */
	movs	pc, lr

SVCHandler:					/* SWI handler */
	movs	pc, lr		/*return to the next instruction after the SWI instruction */


DataAbortHandler:				/* Data Abort handler */
	dsb
	subs	pc, lr, #8			/* points to the instruction that caused the Data Abort exception */

PrefetchAbortHandler:				/* Prefetch Abort handler */
	dsb
	subs	pc, lr, #4			/* points to the instruction that caused the Prefetch Abort exception */

.end
