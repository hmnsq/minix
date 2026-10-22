/* hw_intr handles the hardware dependent part of the interrupts */
#include "hw_intr.h"
#include "bsp_intr.h"

void hw_intr_mask(ကိန်းပြည့် irq){
	bsp_irq_mask(irq);
}

void hw_intr_unmask(ကိန်းပြည့် irq){
	bsp_irq_unmask(irq);
}

void hw_intr_ack(ကိန်းပြည့် irq){};
void hw_intr_used(ကိန်းပြည့် irq){};
void hw_intr_not_used(ကိန်းပြည့် irq){};
void hw_intr_disable_all(void){};
