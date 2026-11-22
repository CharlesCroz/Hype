#include "drivers.h"

struct time
{
    unsigned int seconds;
    unsigned int microsseconds;
};

void time_add(struct time *time, unsigned int microsseconds){
    time->microsseconds += microsseconds;
    while(time->microsseconds >= 1000000){
        time->microsseconds -= 1000000;
        time->seconds++;
    }
}

int time_gte(const struct time *lhs, const struct time *rhs){
    return (lhs->seconds > rhs->seconds) || ((lhs->seconds == rhs->seconds) && (lhs->microsseconds >= rhs->microsseconds));
}

int main(){
    struct time current_time = { 0 };
    struct time target_time = { 0 };
    
    const char greetings[] = "Blinky started partition!\n\r";
    
    uart_send(greetings, sizeof(greetings));
    
    time_get(&target_time.seconds, &target_time.microsseconds);
    time_add(&target_time, 250000);
    
    int cycle = 1;
    while(1){
        time_get(&current_time.seconds, &current_time.microsseconds);
        if(time_gte(&current_time, &target_time)){
            time_add(&target_time, 250000);
            cycle = 3 - cycle;
            led_write(cycle << 2, 0xC);
        }
    }
    
    return 0;
}
