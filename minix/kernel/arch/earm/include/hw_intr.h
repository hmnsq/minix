#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(số_nguyên irq);

void hw_intr_mask(số_nguyên irq);
void hw_intr_unmask(số_nguyên irq);
void hw_intr_ack(số_nguyên irq);
void hw_intr_used(số_nguyên irq);
void hw_intr_not_used(số_nguyên irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
