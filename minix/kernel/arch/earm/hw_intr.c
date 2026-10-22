/* hw_intr handles the hardware dependent part of the interrupts */
#அந்தர்பாவ "hw_intr.h"
#அந்தர்பாவ "bsp_intr.h"

வெறுமை hw_intr_mask(முழு irq){
	bsp_irq_mask(irq);
}

வெறுமை hw_intr_unmask(முழு irq){
	bsp_irq_unmask(irq);
}

வெறுமை hw_intr_ack(முழு irq){};
வெறுமை hw_intr_used(முழு irq){};
வெறுமை hw_intr_not_used(முழு irq){};
வெறுமை hw_intr_disable_all(வெறுமை){};
