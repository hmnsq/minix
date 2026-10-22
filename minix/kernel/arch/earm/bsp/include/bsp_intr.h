#ifndef _BSP_INTR_H_
#define _BSP_INTR_H_

#ifndef __ASSEMBLY__

ശൂന്യം bsp_irq_unmask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം bsp_irq_mask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം bsp_irq_handle(ശൂന്യം);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_INTR_H_ */
