#include "uart.h"
#include "buffer.h"
#include "gic.h"

#define UARTPS_1_BASE           0xE0001000
#define UARTPS_CR_OFFSET        0x00
#define UARTPS_MR_OFFSET        0x04
#define UARTPS_IER_OFFSET       0x08
#define UARTPS_IDR_OFFSET       0x0C
#define UARTPS_IMR_OFFSET       0x10
#define UARTPS_ISR_OFFSET       0x14
#define UARTPS_SR_OFFSET        0x2c
#define UARTPS_FIFO_OFFSET      0x30

#define UARTPS_CR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_CR_OFFSET))
#define UARTPS_MR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_MR_OFFSET))
#define UARTPS_IER_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IER_OFFSET))
#define UARTPS_IDR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IDR_OFFSET))
#define UARTPS_IMR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IMR_OFFSET))
#define UARTPS_ISR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_ISR_OFFSET))
#define UARTPS_SR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_SR_OFFSET))
#define UARTPS_FIFO_REG     (*(volatile int *)(UARTPS_1_BASE + UARTPS_FIFO_OFFSET))


#define UARTPS_SR_TX_FULL   (1 << 4)
#define UARTPS_INT_TXEMPTY  (1 << 3)

static char buffer_data[1024];
static struct rolling_buffer uart_buffer;

int __uart_init(){
    /* Buffer init, innit? */
    buffer_init(&uart_buffer, buffer_data, sizeof(buffer_data));
    
    /* Clear TXEMPTY interrupt */
    UARTPS_IDR_REG = UARTPS_INT_TXEMPTY;

    /* UARTPS1 Interrupt ID is 82, I think */
    gic_activate_irq(82, 3, __uart_hw_callback, 0);

    return 0;
}

static int __tx_fifo_full(){
    return UARTPS_SR_REG & UARTPS_SR_TX_FULL;
}

void __uart_hw_callback(void *param){
    (void) param;
    /* Clear TXEMPTY interrupt */
    UARTPS_IDR_REG = UARTPS_INT_TXEMPTY;

    /* Fill HW buffer */
    char c;
    while(!__tx_fifo_full() && (buffer_pop(&uart_buffer, &c, 1) == 0)){
        UARTPS_FIFO_REG = c;
    }
    
    /* If buffer not empty, set TXEMPTY interrupt */
    if(!buffer_is_empty(&uart_buffer)){
        UARTPS_IER_REG = UARTPS_INT_TXEMPTY;
    }
}

int uart_send(char *text, unsigned int n_char){
    int res = buffer_push(&uart_buffer, text, n_char);
    __uart_hw_callback((void *)0);
    return res;
}
