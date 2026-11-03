#include "kprint.h"
#include "gic.h"
#include "tic.h"
#include "partitions_configuration.h"
#include <stdint.h>

#define REG_LED *((volatile int *)0x40000000)

void mess_table();

void key0(void *arg){
    (void) arg;
    _kprint("\033[14;25HKey 0 - Messed with MMU table");
    REG_LED = REG_LED ^ 0b0001;
    mess_table();
}

void key1(void *arg){
    (void) arg;
    _kprint("\033[14;25HKey 1 - Write to memory      ");
    REG_LED = REG_LED ^ 0b0010;
    *(volatile uint32_t *)(0x00100000) = REG_LED;
}

void key2(void *arg){
    (void) arg;
    _kprint("\033[14;25HKey 2");
    REG_LED = REG_LED ^ 0b0100;
}

void key3(void *arg){
    (void) arg;
    _kprint("\033[14;25HKey 3");
    REG_LED = REG_LED ^ 0b1000;
}


void increase_tic(void *){
    static int tic_count = 0;
    static int second_count = 0;
    tic_count += 1;
    if(tic_count == 100){
        second_count ++;
        _kprint("\033[13;25H");
        _kprint_i32dec(second_count);
        _kprint("s !");
        tic_count = 0;
    }
}

int main()
{
    _kprint("\033[2J\033[0;0H");
    _kprint("===================================\n\r");
    _kprint("==========   HYPE v0.0   ==========\n\r");
    _kprint("===================================\n\r");

    
    _kprint("Partitions (");
    _kprint_i32dec(hype_partitions_count);
    _kprint(")\n\r");
    for(int i = 0; i < hype_partitions_count; i++){
        _kprint(" - ");
        _kprint(hype_partitions[i].name);
        _kprint("\n\r");
    }
    
    _kprint("\n\r\n\r\t\t\tBisous\n\r");

    *((volatile int *)0x40000000) = 15;

    gic_activate_irq(61, 3, key0, 0);
    gic_activate_irq(62, 3, key1, 0);
    gic_activate_irq(63, 3, key2, 0);
    gic_activate_irq(64, 3, key3, 0);
    tic_setup(10, increase_tic, 0);
    gic_enable();


    return 0;
}
