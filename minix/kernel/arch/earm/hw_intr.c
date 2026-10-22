/* hw_intr handles the hardware dependent part of the interrupts */
#অন্তর্ভাব "hw_intr.h"
#অন্তর্ভাব "bsp_intr.h"

শূন্য hw_intr_mask(পূর্ণসংখ্যা irq){
	bsp_irq_mask(irq);
}

শূন্য hw_intr_unmask(পূর্ণসংখ্যা irq){
	bsp_irq_unmask(irq);
}

শূন্য hw_intr_ack(পূর্ণসংখ্যা irq){};
শূন্য hw_intr_used(পূর্ণসংখ্যা irq){};
শূন্য hw_intr_not_used(পূর্ণসংখ্যা irq){};
শূন্য hw_intr_disable_all(শূন্য){};
