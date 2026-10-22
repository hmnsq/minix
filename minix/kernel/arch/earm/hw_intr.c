/* hw_intr handles the hardware dependent part of the interrupts */
#ضمن "hw_intr.h"
#ضمن "bsp_intr.h"

فراغ hw_intr_نقاب(صحيح irq){
	bsp_irq_نقاب(irq);
}

فراغ hw_intr_unmask(صحيح irq){
	bsp_irq_unmask(irq);
}

فراغ hw_intr_ack(صحيح irq){};
فراغ hw_intr_استعمال_ٿيل(صحيح irq){};
فراغ hw_intr_not_استعمال_ٿيل(صحيح irq){};
فراغ hw_intr_disable_all(فراغ){};
