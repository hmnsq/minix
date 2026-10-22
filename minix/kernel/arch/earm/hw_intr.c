/* hw_intr handles the hardware dependent part of the interrupts */
#ਅਨ੍ਤਰ੍ਭਾਵ "hw_intr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_intr.h"

ਖਾਲੀ hw_intr_ਨਕਾਬ(ਪੂਰਨ irq){
	bsp_irq_ਨਕਾਬ(irq);
}

ਖਾਲੀ hw_intr_unmask(ਪੂਰਨ irq){
	bsp_irq_unmask(irq);
}

ਖਾਲੀ hw_intr_ack(ਪੂਰਨ irq){};
ਖਾਲੀ hw_intr_ਵਰਤਿਆ(ਪੂਰਨ irq){};
ਖਾਲੀ hw_intr_not_ਵਰਤਿਆ(ਪੂਰਨ irq){};
ਖਾਲੀ hw_intr_disable_all(ਖਾਲੀ){};
