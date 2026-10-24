#ifndef __PROT_GIC
#define __PROT_GIC

void gic_enable();
void gic_activate_irq(unsigned int id, 
                        int priority, 
                        void (*handler)(void *),
                        void *param);

#endif