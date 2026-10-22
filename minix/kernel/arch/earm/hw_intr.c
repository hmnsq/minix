/* hw_intr handles the hardware dependent part of the interrupts */
#include "hw_intr.h"
#include "bsp_intr.h"

void hw_intr_mask(ចំនួនគត់ irq){
	bsp_irq_mask(irq);
}

void hw_intr_unmask(ចំនួនគត់ irq){
	bsp_irq_unmask(irq);
}

void hw_intr_ack(ចំនួនគត់ irq){};
void hw_intr_used(ចំនួនគត់ irq){};
void hw_intr_not_used(ចំនួនគត់ irq){};
void hw_intr_disable_all(void){};
