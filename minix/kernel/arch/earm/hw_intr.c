/* hw_intr handles the hardware dependent part of the interrupts */
#अन्तर्भाव "hw_intr.h"
#अन्तर्भाव "bsp_intr.h"

शून्य hw_intr_mask(पूर्णांक irq){
	bsp_irq_mask(irq);
}

शून्य hw_intr_unmask(पूर्णांक irq){
	bsp_irq_unmask(irq);
}

शून्य hw_intr_ack(पूर्णांक irq){};
शून्य hw_intr_used(पूर्णांक irq){};
शून्य hw_intr_not_used(पूर्णांक irq){};
शून्य hw_intr_disable_all(शून्य){};
