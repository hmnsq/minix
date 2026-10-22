#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(цео irq);

void hw_intr_mask(цео irq);
void hw_intr_unmask(цео irq);
void hw_intr_ack(цео irq);
void hw_intr_used(цео irq);
void hw_intr_not_used(цео irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
