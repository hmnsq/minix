/* hw_intr handles the hardware dependent part of the interrupts */
#അന്തര്ഭാവ "hw_intr.h"
#അന്തര്ഭാവ "bsp_intr.h"

ശൂന്യം hw_intr_mask(പൂർണ്ണസംഖ്യ irq){
	bsp_irq_mask(irq);
}

ശൂന്യം hw_intr_unmask(പൂർണ്ണസംഖ്യ irq){
	bsp_irq_unmask(irq);
}

ശൂന്യം hw_intr_ack(പൂർണ്ണസംഖ്യ irq){};
ശൂന്യം hw_intr_used(പൂർണ്ണസംഖ്യ irq){};
ശൂന്യം hw_intr_not_used(പൂർണ്ണസംഖ്യ irq){};
ശൂന്യം hw_intr_disable_all(ശൂന്യം){};
