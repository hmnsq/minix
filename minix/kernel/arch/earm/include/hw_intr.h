#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(정수 irq);

void hw_intr_mask(정수 irq);
void hw_intr_unmask(정수 irq);
void hw_intr_ack(정수 irq);
void hw_intr_used(정수 irq);
void hw_intr_not_used(정수 irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
