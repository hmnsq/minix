/* hw_intr handles the hardware dependent part of the interrupts */
#અન્તર્ભાવ "hw_intr.h"
#અન્તર્ભાવ "bsp_intr.h"

શૂન્ય hw_intr_mask(પૂર્ણાંક irq){
	bsp_irq_mask(irq);
}

શૂન્ય hw_intr_unmask(પૂર્ણાંક irq){
	bsp_irq_unmask(irq);
}

શૂન્ય hw_intr_ack(પૂર્ણાંક irq){};
શૂન્ય hw_intr_used(પૂર્ણાંક irq){};
શૂન્ય hw_intr_not_used(પૂર્ણાંક irq){};
શૂન્ય hw_intr_disable_all(શૂન્ય){};
