/* hw_intr handles the hardware dependent part of the interrupts */
#include "hw_intr.h"
#include "bsp_intr.h"

void hw_intr_mask(පූර්ණ_සංඛ්‍යා irq){
	bsp_irq_mask(irq);
}

void hw_intr_unmask(පූර්ණ_සංඛ්‍යා irq){
	bsp_irq_unmask(irq);
}

void hw_intr_ack(පූර්ණ_සංඛ්‍යා irq){};
void hw_intr_used(පූර්ණ_සංඛ්‍යා irq){};
void hw_intr_not_used(පූර්ණ_සංඛ්‍යා irq){};
void hw_intr_disable_all(void){};
