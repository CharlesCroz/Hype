#include "led.h"

#define LED_BASE           0x40000000

#define LED_REG       (*(volatile int *)(LED_BASE))

int __led_read(int * value, int * mask){
    if(mask != (void *)0) *mask = 0xF;
    if(value != (void *)0) *value = LED_REG;
    return 0;
}

int __led_write(int value, int mask){
    int tmp_val = LED_REG;
    tmp_val = tmp_val & (~mask); /* masked bits are nulled */
    tmp_val = tmp_val | (value & mask);
    LED_REG = tmp_val;
    return 0;
}
