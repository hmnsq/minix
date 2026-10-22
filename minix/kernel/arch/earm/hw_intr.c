/* hw_intr handles the hardware dependent part of the interrupts */
#شامل "hw_intr.h"
#شامل "bsp_intr.h"

خالی hw_intr_نقاب(صحیح irq){
	bsp_irq_نقاب(irq);
}

خالی hw_intr_unmask(صحیح irq){
	bsp_irq_unmask(irq);
}

خالی hw_intr_ack(صحیح irq){};
خالی hw_intr_مستعمل(صحیح irq){};
خالی hw_intr_not_مستعمل(صحیح irq){};
خالی hw_intr_disable_all(خالی){};
