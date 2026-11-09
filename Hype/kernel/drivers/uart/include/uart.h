#ifndef HYPE_UART
#define HYPE_UART

/* Private Hype functions, should be hidden from partitions */
int __uart_init();
int __uart_set_interrupt();
int __uart_clear_interrupt();
int __uart_hw_callback(void *param);
int __uart_fill_buffer(unsigned int n_char, char *text);

/* Public Hype functions, should be visible to partitions */
int uart_send(unsigned int n_char, char *text);

#endif
