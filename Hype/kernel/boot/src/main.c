#include "kprint.h"
#include "gic.h"
#include "tic.h"
#include <stdint.h>

#define REG_LED *((volatile int *)0x40000000)

void mess_table();

void print_reg(char *reg_name, unsigned int reg_value){
    _kprint("> ");
    _kprint(reg_name);
    _kprint(" ");
    _kprint_u32hex(reg_value);
    _kprint("\n\r");
}

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
    unsigned int reg;

    _kprint("\033[2J\033[0;0H");
    _kprint("===================================\n\r");
    _kprint("==========   HYPE v0.0   ==========\n\r");
    _kprint("===================================\n\r");
    _kprint("\n\r\n\rSome registers :\n\r");
    
    __asm ("mrc p15, 0, %[result], c2, c0, 0" : [result] "=r" (reg) :);
    print_reg("TTBR0", reg);
    
    // __asm ("mrc p15, 0, %[result], c2, c0, 1" : [result] "=r" (reg) :);
    // print_reg("TTBR1", reg);
    
    // __asm ("mrc p15, 0, %[result], c2, c0, 2" : [result] "=r" (reg) :);
    // print_reg("TTBCR", reg);
    
    _kprint("\n\r\n\r\t\t\tBisous\n\r");

    *((volatile int *)0x40000000) = 15;

    gic_activate_irq(61, 3, key0, 0);
    gic_activate_irq(62, 3, key1, 0);
    gic_activate_irq(63, 3, key2, 0);
    gic_activate_irq(64, 3, key3, 0);
    tic_setup(10, increase_tic, 0);
    gic_enable();
    
    // /* Loop over MMU table entries and check APX bits */
    // uint32_t *mmu = (uint32_t *)0x4000;
    // uint32_t APX_mask = 0x00000003;
    // uint32_t APX_current = mmu[0] & APX_mask;
    // _kprint("\n\n\n\n\n\rMMU Entry ");
    // _kprint_u32hex(mmu[0]);
    // _kprint(" mask ");
    // _kprint_u32hex(APX_current);
    // _kprint("\n\r");
    // for(uint32_t i = 1 ; i < 0x1000; i++){
    //     if((mmu[i] & APX_mask) != APX_current){
    //         APX_current = mmu[i] & APX_mask;
    //         _kprint("MMU Entry ");
    //         _kprint_u32hex(mmu[i]);
    //         _kprint(" mask ");
    //         _kprint_u32hex(APX_current);
    //         _kprint("\n\r");
    //     }
    // }

    return 0;
}
