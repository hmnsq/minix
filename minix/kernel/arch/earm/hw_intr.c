/* hw_intr handles the hardware dependent part of the interrupts */
#अन्तर्भाव "hw_intr.h"
#अन्तर्भाव "bsp_intr.h"

शून्य hw_intr_आवरणम्(पूर्णाङ्क irq){
	bsp_irq_आवरणम्(irq);
}

शून्य hw_intr_unmask(पूर्णाङ्क irq){
	bsp_irq_unmask(irq);
}

शून्य hw_intr_ack(पूर्णाङ्क irq){};
शून्य hw_intr_प्रयुक्तम्(पूर्णाङ्क irq){};
शून्य hw_intr_not_प्रयुक्तम्(पूर्णाङ्क irq){};
शून्य hw_intr_disable_all(शून्य){};
