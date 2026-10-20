/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "xil_printf.h"


void print_reg(char *name, unsigned int reg){
    char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char buff[11] = {'\0'};

    print("> ");
    print(name);
    print(" 0x");

    for(int i = 7; i >= 0; i--){
        buff[7-i] = hex[(reg >> (4*i)) & 0xF];
    }
    buff[8] = '\n';
    buff[9] = '\r';
    buff[10] = '\0';

    print(buff);
}

int main()
{
    unsigned int reg = 0;

    print("\033[2J\033[0;0H");
    print("===================================\n\r");
    print("==========   HYPE v0.0   ==========\n\r");
    print("===================================\n\r");
    print("\n\r\n\rSome registers :\n\r");
    
    __asm ("mrc p15, 0, %[result], c2, c0, 0" : [result] "=r" (reg) :);
    print_reg("TTBR0", reg);
    
    __asm ("mrc p15, 0, %[result], c2, c0, 1" : [result] "=r" (reg) :);
    print_reg("TTBR1", reg);
    
    __asm ("mrc p15, 0, %[result], c2, c0, 2" : [result] "=r" (reg) :);
    print_reg("TTBCR", reg);
    print("\n\r\n\r\t\t\tBisous\n\r");


    return 0;
}
