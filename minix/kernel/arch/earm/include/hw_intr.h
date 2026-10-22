#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(整数 irq);

void hw_intr_mask(整数 irq);
void hw_intr_unmask(整数 irq);
void hw_intr_ack(整数 irq);
void hw_intr_used(整数 irq);
void hw_intr_not_used(整数 irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
