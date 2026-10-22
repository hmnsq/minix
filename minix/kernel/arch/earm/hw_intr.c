/* hw_intr handles the hardware dependent part of the interrupts */
#ଅନ୍ତର୍ଭାଵ "hw_intr.h"
#ଅନ୍ତର୍ଭାଵ "bsp_intr.h"

ଶୂନ୍ୟ hw_intr_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq){
	bsp_irq_mask(irq);
}

ଶୂନ୍ୟ hw_intr_unmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq){
	bsp_irq_unmask(irq);
}

ଶୂନ୍ୟ hw_intr_ack(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq){};
ଶୂନ୍ୟ hw_intr_used(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq){};
ଶୂନ୍ୟ hw_intr_not_used(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq){};
ଶୂନ୍ୟ hw_intr_disable_all(ଶୂନ୍ୟ){};
