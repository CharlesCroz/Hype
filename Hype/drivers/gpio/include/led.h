#ifndef HYPE_LED
#define HYPE_LED

/* Private Hype functions, should be hidden from partitions */
int __led_read(int * value, int * mask);
int __led_write(int value, int mask);

#endif
