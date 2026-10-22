#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(ціле irq);

void hw_intr_mask(ціле irq);
void hw_intr_unmask(ціле irq);
void hw_intr_ack(ціле irq);
void hw_intr_used(ціле irq);
void hw_intr_not_used(ціле irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
