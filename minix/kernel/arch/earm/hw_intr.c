/* hw_intr handles the hardware dependent part of the interrupts */
#include "hw_intr.h"
#include "bsp_intr.h"

void hw_intr_mask(ціле irq){
	bsp_irq_mask(irq);
}

void hw_intr_unmask(ціле irq){
	bsp_irq_unmask(irq);
}

void hw_intr_ack(ціле irq){};
void hw_intr_used(ціле irq){};
void hw_intr_not_used(ціле irq){};
void hw_intr_disable_all(void){};
