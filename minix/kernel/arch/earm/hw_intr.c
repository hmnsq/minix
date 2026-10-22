/* hw_intr handles the hardware dependent part of the interrupts */
#include "hw_intr.h"
#include "bsp_intr.h"

void hw_intr_mask(puur_naa_nka irq){
	bsp_irq_mask(irq);
}

void hw_intr_unmask(puur_naa_nka irq){
	bsp_irq_unmask(irq);
}

void hw_intr_ack(puur_naa_nka irq){};
void hw_intr_used(puur_naa_nka irq){};
void hw_intr_not_used(puur_naa_nka irq){};
void hw_intr_disable_all(void){};
