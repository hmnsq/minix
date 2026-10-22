/* hw_intr handles the hardware dependent part of the interrupts */
#شامل "hw_intr.h"
#شامل "bsp_intr.h"

تهی hw_intr_mask(صحیح irq){
	bsp_irq_mask(irq);
}

تهی hw_intr_unmask(صحیح irq){
	bsp_irq_unmask(irq);
}

تهی hw_intr_ack(صحیح irq){};
تهی hw_intr_used(صحیح irq){};
تهی hw_intr_not_used(صحیح irq){};
تهی hw_intr_disable_all(تهی){};
