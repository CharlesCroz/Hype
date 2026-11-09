#include "uart.h"
#include "buffer.h"
#include "gic.h"

#define UARTPS_1_BASE           0xE0001000
#define UARTPS_SR_OFFSET        0x2c
#define UARTPS_FIFO_OFFSET      0x30
#define UARTPS_SR_TX_FULL_BIT   0b10000



char __buffer_data[1024];
struct rolling_buffer __uart_buffer;

int __uart_init(){
    buffer_init(&__uart_buffer, __buffer_data, sizeof(__buffer_data));

}

int uart_send(unsigned int n_char, char *text){
    return 0;
}
