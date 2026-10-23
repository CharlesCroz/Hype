#ifndef __PROT_GIC
#define __PROT_GIC

void gic_enable();
void gic_activate_irq(int id, int priority);

#endif