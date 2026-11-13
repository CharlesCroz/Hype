#include "xparameters.h"
#include "kprint.h"
#include "uart.h"
#include <stdarg.h>


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

void _kprint(const char *string){
    while(*string){
        _kprint_putchar(*string);
        string++;
    }
}
void _sprint_u32dec(char *buff, unsigned int val){
    int digit_count = 0;
    char digits[10];

    while(val){
        digits[digit_count] = val % 10;
        val = val / 10;
        digit_count++;
    }

    if(digit_count == 0){
        *(buff++) = '0';
    }else{
        while(digit_count){
            digit_count--;
            *(buff++) = ('0' + digits[digit_count]);
        }
    }

    *buff = '\0';
}

void _sprint_i32dec(char *buff, int val){
    int isneg = (val < 0);

    if(isneg){
        val = -val;
        *(buff++) = '-';
    }
    _sprint_u32dec(buff, val);
}

void _sprint_u32hex(char *buff, unsigned int val){
    static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int shift;

    shift = sizeof(val) * 8;
    while(shift){
        shift = shift - 4;
        *(buff++) = hex[(val >> shift) & 0xF];
    }
    *buff = '\0';

    return;
}

void _kprintf(const char *pattern, ...){
    va_list args;
    char built_string[1024];
    char number_buffer[16];
    int at = 0;
    const char *at_n = number_buffer;
    
    va_start(args, pattern);
    
    while(*pattern){
        if(*pattern == '%'){
            pattern++;

            /* Process thing */
            switch (*pattern){
                case 'x':
                    _sprint_u32hex(number_buffer, va_arg(args, unsigned int));
                    at_n = number_buffer;
                    break;
                    case 'u':
                    _sprint_u32dec(number_buffer, va_arg(args, unsigned int));
                    at_n = number_buffer;
                    break;
                    case 'd':
                    case 'i':
                    _sprint_i32dec(number_buffer, va_arg(args, int));
                    at_n = number_buffer;
                    break;
                case 's':
                    at_n = va_arg(args, const char *);
                    break;
                case '%':
                    number_buffer[0] = '%';
                    number_buffer[1] = '\0';
                    break;
                
                default:
                    break;
            }
            while(*at_n && at < sizeof(built_string)){
                built_string[at] = *(at_n++);
                at++;
            }
        }else{
            built_string[at] = *pattern;
            at++;
        }

        if(at == sizeof(built_string)){
            break;
        }
        pattern++;
    }
    va_end(args);

    uart_send(built_string, at);
}


void _kprint_i32dec(int val){
    char buff[16];
    _sprint_i32dec(buff, val);
    _kprint(buff);
}

void _kprint_u32dec(unsigned int val){
    char buff[16];
    _sprint_u32dec(buff, val);
    _kprint(buff);
}

void _kprint_u32hex(unsigned int val){
    char buff[16];
    _sprint_u32hex(buff, val);
    _kprint(buff);
}

