.org 0

.text

.globl _vector_table

.section .vectors
_vector_table:
	B	_boot
	B	Undefined
	B	decorate_syscall
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	/* Placeholder for address exception vector*/
	B	decorate_irq
/*	B	FIQHandler Undefining this so the exception directly points to FIQ handler code */

FIQHandler:			            /* FIQ vector handler */
	b 		FIQHandler			/* Infinite loop.... for now ! */
	subs	pc, lr, #4			/* adjust return */

decorate_syscall:
	push 	{r0-r12, lr}
	bl 		handle_syscall
	pop 	{r0-r12, lr}
	movs	pc, lr

decorate_irq:
	push 	{r0-r12, lr}
	bl 		handle_irq
	pop 	{r0-r12, lr}
	subs	pc, lr, #4

Undefined:					/* Undefined handler */
	movs	pc, lr

DataAbortHandler:				/* Data Abort handler */
	dsb
	push	{lr}
	bl		handle_data_abort
	pop 	{lr}
	@ subs	pc, lr, #8			/* points to the instruction that caused the Data Abort exception */
dead:
	b dead

PrefetchAbortHandler:				/* Prefetch Abort handler */
	dsb
	subs	pc, lr, #4			/* points to the instruction that caused the Prefetch Abort exception */

.end
