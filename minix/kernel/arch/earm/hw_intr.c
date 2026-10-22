/* hw_intr handles the hardware dependent part of the interrupts */
#అన్తర్భావ "hw_intr.h"
#అన్తర్భావ "bsp_intr.h"

శూన్య hw_intr_ముసుగు(పూర్ణాంకం irq){
	bsp_irq_ముసుగు(irq);
}

శూన్య hw_intr_unmask(పూర్ణాంకం irq){
	bsp_irq_unmask(irq);
}

శూన్య hw_intr_ack(పూర్ణాంకం irq){};
శూన్య hw_intr_వాడిన(పూర్ణాంకం irq){};
శూన్య hw_intr_not_వాడిన(పూర్ణాంకం irq){};
శూన్య hw_intr_disable_all(శూన్య){};
