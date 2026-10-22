#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(цяло irq);

void hw_intr_mask(цяло irq);
void hw_intr_unmask(цяло irq);
void hw_intr_ack(цяло irq);
void hw_intr_used(цяло irq);
void hw_intr_not_used(цяло irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
