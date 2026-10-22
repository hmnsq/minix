#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(целое irq);

void hw_intr_mask(целое irq);
void hw_intr_unmask(целое irq);
void hw_intr_ack(целое irq);
void hw_intr_used(целое irq);
void hw_intr_not_used(целое irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
