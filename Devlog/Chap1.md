# Chapitre 1 - Interruptions

Ok, I can compile. I can boot and send over UART (though it's Xilinx code, not mine). Now, time to dig into interrupts, "external" (aka buttons) and internal (the tasty ones, clocks, memory error, and so on...).

## Back to asm

Basically, when an interrupt/exception is triggeredm some registers are saved/banked, then the code jumps to the entry in the vector table (typically at 0x0, but it can be mapped high at 0xFFFF_0000) corresponding to the interrupt :
|Vector offset|Exception|Mode|CPSR Mask|Return|
|-|-|-|-|-|
|0x00|Reset|Supervisor|F=1, I=1|-|
|0x04|Undefined instruction|Undef|I=1|`MOVS PC, R14` -> Next instruction|
|0x08|Supervisor Call|Supervisor|I=1|`MOVS PC, R14` -> Next instruction|
|0x0C|Prefectch Abort|Abort|I=1|`SUBS PC, R14, #4` -> Aborting instruction|
|0x10|Data Abort|Abort|I=1|`SUBS PC, R14, #8` -> Aborting instruction if precise|
|0x14|Not Used|Hyp|-|-|
|0x18|IRQ|IRQ|I=1|`SUBS PC, R14, #4` Next instruction|
|0x1C|FIQ|FIQ|F=1, I=1|`SUBS PC, R14, #4` Next instruction|

What I intended to do : use Xilinx functions to register a function that change the LEDs pattern when I hit a button. Did it work ? Obviously not. 

And well... ironically, looking more into the instruction being run at startup, I decided to go for boot code instead of interrupts. Because after all.... Boot code is the first thing that runs !

## Reworking the base

So... Best way to understand is to do, right ?

So here we are, makeing a boot.S that configures the CPU and CP15, initializes stack pointer and link return value, and here we go !

Oh, and I made a dummy exception vector table (for now).

Oh, and a very cool _exit loop. Ok, not really, but it's mine, **I** made the code jumps at the end of the main. No, seriously, I do feel a sizeable amount of pride in it.

```assembly
.org 0

.text

.globl _exit

.section ._exit,"ax"
_exit:
	b _exit

.end
```

To be honnest, the rest is more interesting. For instance, using AMD Xilinx UG-585, I got the useful register to send bytes over UART.

```C
static void _kprint_putchar(char c){
    volatile unsigned int *reg_uart_fifo_status = (void *)(XPAR_PS7_UART_1_BASEADDR + XUARTPS_SR_OFFSET);
    volatile unsigned int *reg_uart_fifo =        (void *)(XPAR_PS7_UART_1_BASEADDR + XUARTPS_FIFO_OFFSET);

    while(*reg_uart_fifo_status & XUARTPS_SR_TX_FULL_BIT)
        ; /* Wait for UART TX FIFO to be "not full" */
    *reg_uart_fifo = c; /* put char in UART TX FIFO*/
}

void _kprint(char *string){
    while(*string){
        _kprint_putchar(*string);
        string++; /* _kprint_putchar(*(string++)); in one line is meh */  
    }
}
```

The compiler used the keywork static to treat `_kprint_putchar` as an inlined function. Who am I to judge ? Actually, it' s just a few instructions, often repeated, so I'm glad it saved the jumps.
