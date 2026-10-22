#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(จำนวนเต็ม irq);

void hw_intr_mask(จำนวนเต็ม irq);
void hw_intr_unmask(จำนวนเต็ม irq);
void hw_intr_ack(จำนวนเต็ม irq);
void hw_intr_used(จำนวนเต็ม irq);
void hw_intr_not_used(จำนวนเต็ม irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
