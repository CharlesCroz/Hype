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


void increase_tic(void *param){
    (void) param;
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

struct scheduler_status {
    unsigned int current_ms;
    unsigned int current_slot;
} scheduler_status;

void scheduler(void *param){
    struct scheduler_status *sched_status = (struct scheduler_status *) param;

    sched_status->current_ms += 1;
    if(sched_status->current_ms == hype_scheduling[sched_status->current_slot].to_ms){
        sched_status->current_slot++;
        if(sched_status->current_slot == hype_scheduling_count){
            sched_status->current_slot = 0;
            sched_status->current_ms = 0;
        }
        _kprint("\033[13;25H");
        _kprint(hype_partitions[hype_scheduling[sched_status->current_slot].partition_id].name);
        _kprint("  ");
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
    scheduler_status = (struct scheduler_status){
        .current_ms = hype_scheduling[hype_scheduling_count].to_ms - 1,
        .current_slot = hype_scheduling_count - 1
    };
    tic_setup(1, scheduler, &scheduler_status);
    // tic_setup(10, increase_tic, 0);
    gic_enable();


    return 0;
}
