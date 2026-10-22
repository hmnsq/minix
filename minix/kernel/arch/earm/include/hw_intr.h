#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(ចំនួនគត់ irq);

void hw_intr_mask(ចំនួនគត់ irq);
void hw_intr_unmask(ចំនួនគត់ irq);
void hw_intr_ack(ចំនួនគត់ irq);
void hw_intr_used(ចំនួនគត់ irq);
void hw_intr_not_used(ចំនួនគត់ irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
