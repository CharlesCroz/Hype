#include "tic.h"
#include "gic.h"
#include "xparameters.h"
#include "kprint.h"
#include <stdint.h>

#define REG_LOAD    *(volatile int *)(XPAR_PS7_SCUTIMER_0_BASEADDR + 0x0)
#define REG_COUNT   *(volatile int *)(XPAR_PS7_SCUTIMER_0_BASEADDR + 0x4)
#define REG_CONTROL *(volatile int *)(XPAR_PS7_SCUTIMER_0_BASEADDR + 0x8)
#define REG_STATUS  *(volatile int *)(XPAR_PS7_SCUTIMER_0_BASEADDR + 0xC)

void (*handle_tic_function)(void *) = 0;
void *handle_tic_param = 0;

void handle_tic(void *param){
    (void) param;
    handle_tic_function(handle_tic_param);
    REG_STATUS = 1;
}

void tic_setup(int ms_duration, void (*callback)(void *), void *param){
    unsigned int prescaler; 
    unsigned int load_value;
    /* Prescale to 249, so F_timer is F_CPU/(2*(249+1)) = F_CPU/500 */
    /* For F_CPU = 666MHz, we get F_Fimer = 1.33MHz */
    /* So, duration = load_value * F_timer, 
    i.e load_value = duration (s) * F_CPU/(2 * (prescaler + 1)) = duration (ms) * F_CPU/(2000 * (prescaler + 1))  */
    prescaler = 255;
    load_value = (uint64_t)ms_duration * (uint64_t)(XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ) / (2000 * (1 + prescaler));

    /* Set call back (extra care to clean REG_STATUS) */
    handle_tic_function = callback;
    handle_tic_param = param;
    gic_activate_irq(29, 2, handle_tic, 0);

    /* Just in case, write a 1 to Status reg to clear */
    REG_STATUS = 1;
    /* Set load value */
    REG_LOAD = load_value;
    /* Set count to load */
    REG_COUNT = load_value;
    /* Set controle and here we go ! */
    REG_CONTROL = (prescaler << 8) | 0b111;
}
