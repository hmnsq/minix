#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#അന്തര്ഭാവ "kernel/kernel.h"
ശൂന്യം irq_handle(പൂർണ്ണസംഖ്യ irq);

ശൂന്യം hw_intr_mask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം hw_intr_unmask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം hw_intr_ack(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം hw_intr_used(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം hw_intr_not_used(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം hw_intr_disable_all(ശൂന്യം);

#endif /* __HW_INTR_ARM_H__ */
