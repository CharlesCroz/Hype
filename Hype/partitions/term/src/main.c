#include "drivers.h"

int strlen(const char *str){
    int res = 0;

    while(*(str++)){
        res++;
    }
    return res;
}


void print(const char *str){
    uart_send(str, strlen(str));
}


int main(){
    char c;
    unsigned int seconds;
    int target = 1;

    print("Partition term started!\n\r");
    print("Press [q] to exit\r\n");

    do{
        c = '\0';
        uart_recv(&c, 1);
        if(c == '\r'){
            char nlbr[] = "\n\r";
            uart_send(nlbr, 2);
        }else if(c == 0x7F){
            char backspace[] = "\b \b";
            uart_send(backspace, 3);
        }else if(c >= 0x20 && c < 0x7F){
            uart_send(&c, 1);
        }

        time_get(&seconds, (void *) 0);
        if(seconds > target){
            led_write(target, 0xF);
            target++;
        }
    }while(c != 'q');

    print("\r\n");

    return 42;
} /* main */
