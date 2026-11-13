#ifndef HYPE_UART
#define HYPE_UART

/* Private Hype functions, should be hidden from partitions */
int __uart_init();
void __uart_hw_callback(void *param);

/* Public Hype functions, should be visible to partitions */
int uart_send(char *text, unsigned int n_char);

#endif
