/* hw_intr handles the hardware dependent part of the interrupts */
#समावेश "hw_intr.h"
#समावेश "bsp_intr.h"

व्योम hw_intr_mask(पूर्णांक irq){
	bsp_irq_mask(irq);
}

व्योम hw_intr_unmask(पूर्णांक irq){
	bsp_irq_unmask(irq);
}

व्योम hw_intr_ack(पूर्णांक irq){};
व्योम hw_intr_used(पूर्णांक irq){};
व्योम hw_intr_not_used(पूर्णांक irq){};
व्योम hw_intr_disable_all(व्योम){};
