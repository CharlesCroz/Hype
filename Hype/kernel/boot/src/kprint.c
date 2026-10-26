#include "xparameters.h"
#include "kprint.h"

#define XUARTPS_SR_OFFSET       0x2c
#define XUARTPS_FIFO_OFFSET     0x30
#define XUARTPS_SR_TX_FULL_BIT  0b10000

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
        string++;
    }
}

void _kprint_i32dec(int val){
    int isneg = (val < 0);

    if(isneg){
        val = -val;
        _kprint_putchar('-');
    }
    _kprint_u32dec(val);
}

void _kprint_u32dec(unsigned int val){
    int digit_count = 0;
    char digits[10];

    while(val){
        digits[digit_count] = val % 10;
        val = val / 10;
        digit_count++;
    }

    if(digit_count == 0){
        _kprint_putchar('0');
    }else{
        while(digit_count){
            digit_count--;
            _kprint_putchar('0' + digits[digit_count]);
        }
    }
}

void _kprint_u32hex(unsigned int val){
    static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int shift;

    _kprint_putchar('0');
    _kprint_putchar('x');

    shift = sizeof(val) * 8;
    while(shift){
        shift = shift - 4;
        _kprint_putchar(hex[(val >> shift) & 0xF]);
    }

    return;
}
