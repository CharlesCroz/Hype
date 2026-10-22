#include "kprint.h"

void print_reg(char *reg_name, unsigned int reg_value){
    _kprint("> ");
    _kprint(reg_name);
    _kprint(" ");
    _kprint_u32hex(reg_value);
    _kprint("\n\r");
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
    
    __asm ("mrc p15, 0, %[result], c2, c0, 1" : [result] "=r" (reg) :);
    print_reg("TTBR1", reg);
    
    __asm ("mrc p15, 0, %[result], c2, c0, 2" : [result] "=r" (reg) :);
    print_reg("TTBCR", reg);
    _kprint("\n\r\n\r\t\t\tBisous\n\r");

    *((int *)0x40000000) = 15;

    return 0;
}
