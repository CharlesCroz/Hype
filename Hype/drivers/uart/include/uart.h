#ifndef HYPE_UART
#define HYPE_UART

/* Private Hype functions, should be hidden from partitions */
int __uart_init();
void __uart_hw_callback(void *param);
int __uart_send(const char *text, unsigned int len);

#endif
