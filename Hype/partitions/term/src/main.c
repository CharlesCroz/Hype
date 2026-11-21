#include "drivers.h"

int strlen(const char *str){
    int res = 0;
    while (*(str++))
    {
        res++;
    }
    return res;
}

void print(const char *str){
    uart_send(str, strlen(str));
}

int main(){
    int mask, value;
    char c;
    unsigned int seconds;
    int target = 1;

    print("Hello from term partition!\n\r");
    led_write(0x2, 0xF);
    led_read(&value, &mask);

    print("Value: ");
    for(int i = 0; i < value; i++){
        print("X ");
    }
    print("\r\nPress [q] to exit\r\n");
    
    do{
        c = '\0';
        uart_recv(&c, 1);
        if(c != '\0'){
            uart_send(&c, 1);
        }
        if(c == '\r'){
            c = '\n';
            uart_send(&c, 1);
        }

        time_get(&seconds, (void *)0);
        if(seconds > target){
            led_write(target, 0xF);
            target++;
        }

    }while(c != 'q');

    print("\r\n");

    return 42;
}