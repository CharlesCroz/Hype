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
    print("Hello from term partition!\n\r");
    return 42;
}