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
#define UARTPS_RXTOUT_OFFSET    0x1C
#define UARTPS_RXWM_OFFSET      0x20
#define UARTPS_SR_OFFSET        0x2c
#define UARTPS_FIFO_OFFSET      0x30

#define UARTPS_CR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_CR_OFFSET))           /* Control Register */
#define UARTPS_MR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_MR_OFFSET))           /* Mode Register */
#define UARTPS_IER_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IER_OFFSET))          /* Interrupt Enable */
#define UARTPS_IDR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IDR_OFFSET))          /* Interrupt Disable */
#define UARTPS_IMR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_IMR_OFFSET))          /* Interrupt Mask */
#define UARTPS_ISR_REG      (*(volatile int *)(UARTPS_1_BASE + UARTPS_ISR_OFFSET))          /* Interrupt Status (wtc) */
#define UARTPS_RXTOUT_REG   (*(volatile int *)(UARTPS_1_BASE + UARTPS_RXTOUT_OFFSET))       /* RX Timeout */
#define UARTPS_RXWM_REG     (*(volatile int *)(UARTPS_1_BASE + UARTPS_RXWM_OFFSET))         /* RX Trigger Level */
#define UARTPS_SR_REG       (*(volatile int *)(UARTPS_1_BASE + UARTPS_SR_OFFSET))           /* Modem Status */
#define UARTPS_FIFO_REG     (*(volatile int *)(UARTPS_1_BASE + UARTPS_FIFO_OFFSET))         /* RX/TX FIFO */


#define UARTPS_SR_TX_FULL   (1 << 4)
#define UARTPS_SR_RX_EMPTY  (1 << 1)
#define UARTPS_INT_TXEMPTY  (1 << 3)
#define UARTPS_INT_RXLEVEL  (1 << 0)
#define UARTPS_INT_RXTOUT   (1 << 8)

static char send_buffer_data[1024];
static struct rolling_buffer uart_send_buffer;
static char recv_buffer_data[1024];
static struct rolling_buffer uart_recv_buffer;

int __uart_init(){
    /* Buffer init, innit? */
    buffer_init(&uart_send_buffer, send_buffer_data, sizeof(send_buffer_data));
    buffer_init(&uart_recv_buffer, recv_buffer_data, sizeof(recv_buffer_data));
    
    /* Clear TXEMPTY interrupt */
    UARTPS_IDR_REG = UARTPS_INT_TXEMPTY;

    /* Set RX interrupt sensitivity to 1, and enable interrupt */
    UARTPS_RXWM_REG = 1;
    UARTPS_IER_REG = UARTPS_INT_RXLEVEL;
    /* Set RX Timeout to 10, and enable interrupt */
    UARTPS_RXTOUT_REG = 10;
    UARTPS_IER_REG = UARTPS_INT_RXTOUT;

    /* UARTPS1 Interrupt ID is 82, I think */
    gic_activate_irq(82, 3, __uart_hw_callback, 0);

    return 0;
}

static int __tx_fifo_full(){
    return UARTPS_SR_REG & UARTPS_SR_TX_FULL;
}

static int __rx_fifo_empty(){
    return UARTPS_SR_REG & UARTPS_SR_RX_EMPTY;
}

void __handle_tx_empty(){
    /* Disable TXEMPTY interrupt */
    UARTPS_IDR_REG = UARTPS_INT_TXEMPTY;

    /* Fill HW buffer */
    char c;
    while(!__tx_fifo_full() && (buffer_pop(&uart_send_buffer, &c, 1) == 0)){
        UARTPS_FIFO_REG = c;
    }
    
    /* If buffer not empty, set TXEMPTY interrupt */
    if(!buffer_is_empty(&uart_send_buffer)){
        UARTPS_IER_REG = UARTPS_INT_TXEMPTY;
    }
}

void __handle_rx_level(){
    /* Copy HW buffer to SW buffer */
    while(!__rx_fifo_empty()){
        char c = UARTPS_FIFO_REG;
        buffer_push(&uart_recv_buffer, &c, 1);
    }

    /* Clear interrupt flag */
    UARTPS_ISR_REG = UARTPS_INT_RXLEVEL | UARTPS_INT_RXTOUT;    
}

void __uart_hw_callback(void *param){
    (void) param;
    __handle_rx_level();
    __handle_tx_empty();
}

int __uart_send(const char *text, unsigned int n_char){
    int res = buffer_push(&uart_send_buffer, text, n_char);
    __uart_hw_callback((void *)0);
    return res;
}

int __uart_recv(char *text, unsigned int n_char){
    int res = buffer_pop(&uart_recv_buffer, text, n_char);
    return res;
}
