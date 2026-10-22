/* hw_intr handles the hardware dependent part of the interrupts */
#ضمن "hw_intr.h"
#ضمن "bsp_intr.h"

فراغ hw_intr_mask(صحيح irq){
	bsp_irq_mask(irq);
}

فراغ hw_intr_unmask(صحيح irq){
	bsp_irq_unmask(irq);
}

فراغ hw_intr_ack(صحيح irq){};
فراغ hw_intr_used(صحيح irq){};
فراغ hw_intr_not_used(صحيح irq){};
فراغ hw_intr_disable_all(فراغ){};
