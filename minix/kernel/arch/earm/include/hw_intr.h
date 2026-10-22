#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(ကိန်းပြည့် irq);

void hw_intr_mask(ကိန်းပြည့် irq);
void hw_intr_unmask(ကိန်းပြည့် irq);
void hw_intr_ack(ကိန်းပြည့် irq);
void hw_intr_used(ကိန်းပြည့် irq);
void hw_intr_not_used(ကိန်းပြည့် irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
