#include "kprint.h"
#include "gic.h"
#include "tic.h"
#include "syscall.h"
#include "uart.h"
#include "partitions_configuration.h"
#include <stdint.h>
#include <string.h>

#define REG_LED *((volatile int *)0x40000000)

void key(void *arg){
    _kprint("\033[14;25HKey pressed");
    REG_LED = REG_LED ^ ((int) arg);
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
    __uart_init();

    _kprintf("\033[2J\033[0;0H"
             "===================================\n\r"
             "==========   HYPE v0.0   ==========\n\r"
             "===================================\n\r");

    _kprintf("Partitions (%i)\n\r", hype_partitions_count);
    for(int i = 0; i < hype_partitions_count; i++){
        _kprintf(" - %s\n\r", hype_partitions[i].name);
    }
    
    _kprintf("\n\r\n\r\t\t\tBisous\n\r");

    *((volatile int *)0x40000000) = 15;

    gic_activate_irq(61, 3, key, (void *)1);
    gic_activate_irq(62, 3, key, (void *)2);
    gic_activate_irq(63, 3, key, (void *)4);
    gic_activate_irq(64, 3, key, (void *)8);


    scheduler_status = (struct scheduler_status){
        .current_ms = hype_scheduling[hype_scheduling_count].to_ms - 1,
        .current_slot = hype_scheduling_count - 1
    };
    tic_setup(1, scheduler, &scheduler_status);
    // tic_setup(10, increase_tic, 0);
    gic_enable();

    /* Wait for UART TX to be empty before we start messing with things */
    while(((*(volatile unsigned int *)0xE000102C) & (1<<3)) == 0){
        /* nop */
    }

    const char hello[] = "Hello\n\r";
    __syscall2(SYSCALL_UART_SEND, (void *)hello, (void *)sizeof(hello));
    __syscall0(1);
    __syscall0(3);

    return 0;
}
