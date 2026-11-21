#include "time.h"
#include "xparameters.h"
#include <stdint.h>

#define GT_BASE        0xF8F00200
#define GT_LO_REG      (*(volatile uint32_t *) (GT_BASE + 0x00))
#define GT_HI_REG      (*(volatile uint32_t *) (GT_BASE + 0x04))
#define GT_CONTROL_REG (*(volatile uint32_t *) (GT_BASE + 0x08))

#define GT_FREQ        (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)

uint64_t __time_init_value;

static uint64_t read_globaltimer(){
    uint32_t hi_reg_A, hi_reg_B, low_reg;

    do {
        hi_reg_A = GT_HI_REG;
        low_reg  = GT_LO_REG;
        hi_reg_B = GT_HI_REG;
    } while(hi_reg_A != hi_reg_B);

    return ((uint64_t) hi_reg_A << 32) | low_reg;
}


void __time_init(){
    GT_CONTROL_REG    = 1;
    __time_init_value = read_globaltimer();
}


int __time_get(unsigned int *seconds, unsigned int *microseconds){
    uint64_t gt_count;
    uint64_t __microseconds;

    if(seconds != (void *) 0 || microseconds != (void *) 0){
        gt_count       = read_globaltimer() - __time_init_value;
        __microseconds = gt_count / (GT_FREQ / 1000000ul);

        if(seconds != (void *) 0){
            *seconds = (unsigned int) (__microseconds / 1000000ul);
        }
        if(microseconds != (void *) 0){
            *microseconds = (unsigned int) (__microseconds % 1000000ul);
        }
    }

    return 0;
}
