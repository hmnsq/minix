#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(puur_naa_nka irq);

void hw_intr_mask(puur_naa_nka irq);
void hw_intr_unmask(puur_naa_nka irq);
void hw_intr_ack(puur_naa_nka irq);
void hw_intr_used(puur_naa_nka irq);
void hw_intr_not_used(puur_naa_nka irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
