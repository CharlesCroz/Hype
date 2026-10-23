#include "gic.h"
#include "xparameters.h"
#include "kprint.h"

/* Distributor registers, XPAR_SCUGIC_0_DIST_BASEADDR */
#define GIC_ICDDCR_OFFSET   0x0
#define GIC_ICDISERn_OFFSET 0x100
#define GIC_ICDICERn_OFFSET 0x180
#define GIC_ICDIPRn_OFFSET  0x400
#define GIC_ICDIPTRn_OFFSET 0x800
#define GIC_ICDICFRn_OFFSET 0xC00

/* Controller ? register, XPAR_SCUGIC_0_CPU_BASEADDR */
#define GIC_ICCICR_OFFSET   0x0
#define GIC_ICCPMR_OFFSET   0x4
#define GIC_ICCIAR_OFFSET   0xC
#define GIC_ICCEOIR_OFFSET  0x10

void handle_irq(){
    volatile int irq_id;
    /* Get IRQ ID*/
    irq_id = *(volatile int *)(XPAR_SCUGIC_0_CPU_BASEADDR + GIC_ICCIAR_OFFSET);
    /* Clear IRQ ID*/
    *(volatile int *)(XPAR_SCUGIC_0_CPU_BASEADDR + GIC_ICCEOIR_OFFSET) = irq_id;
    _kprint("IRQ ");
    _kprint_u32dec(irq_id);
    _kprint(" triggered and cleared\n\r");
}

void gic_enable(){
    /* Enable SCU (secure and non secure)*/
    *(volatile int *)(XPAR_PS7_SCUC_0_S_AXI_BASEADDR) = 0x3;
    /* Set mask to all priority ? */
    *(volatile int *)(XPAR_SCUGIC_0_CPU_BASEADDR + GIC_ICCPMR_OFFSET) = 0x1F << 3;
    /* Enable signaling of interrupts */
    *(volatile int *)(XPAR_SCUGIC_0_CPU_BASEADDR + GIC_ICCICR_OFFSET) = 0x1;
    /* Enable distribution of interrupts */
    *(volatile int *)(XPAR_SCUGIC_0_DIST_BASEADDR + GIC_ICDDCR_OFFSET) = 0x1;

    /* Allow IRQ by clearing CSPR bit 7 */
    __asm(  "mrs ip, CPSR\n\r"
            "bic ip, #(1 << 7)\n\r"
            "msr CPSR, ip"
        );
}

void gic_activate_irq(int id, int priority){
    int reg_address;
    int reg_value;

    _kprint("\n\r");
    /* Set enable */
    reg_address = XPAR_SCUGIC_0_DIST_BASEADDR + GIC_ICDISERn_OFFSET + (id / 32) * 4;
    reg_value = 1 << (id % 32);
    *(volatile int *)(reg_address) = reg_value;
    
    /* Set priority */
    reg_address = XPAR_SCUGIC_0_DIST_BASEADDR + GIC_ICDIPRn_OFFSET + (id / 4) * 4;
    reg_value = *(volatile int *)(reg_address);
    reg_value &= ~(0xFF << (id % 4) * 8);
    reg_value |= (priority & 0x1F) << ((id % 4) * 8 + 3);
    *(volatile int *)(reg_address) = reg_value;
    
    /* Send to CPU 0 */
    reg_address = XPAR_SCUGIC_0_DIST_BASEADDR + GIC_ICDIPTRn_OFFSET + (id / 4) * 4;
    reg_value = *(volatile int *)(reg_address);
    reg_value &= ~(0xFF << (id % 4) * 8);
    reg_value |= 1 << ((id % 4) * 8);
    *(volatile int *)(reg_address) = reg_value;
    
    /* Set Sensitivity */
    reg_address = XPAR_SCUGIC_0_DIST_BASEADDR + GIC_ICDICFRn_OFFSET + (id / 16) * 4;
    reg_value = *(volatile int *)(reg_address);
    reg_value &= ~(0b11 << (id % 16) * 2);
    reg_value |= 1 << ((id % 16) * 2  + 1);
    *(volatile int *)(reg_address) = reg_value;
}
