#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(පූර්ණ_සංඛ්‍යා irq);

void hw_intr_mask(පූර්ණ_සංඛ්‍යා irq);
void hw_intr_unmask(පූර්ණ_සංඛ්‍යා irq);
void hw_intr_ack(පූර්ණ_සංඛ්‍යා irq);
void hw_intr_used(පූර්ණ_සංඛ්‍යා irq);
void hw_intr_not_used(පූර්ණ_සංඛ්‍යා irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
