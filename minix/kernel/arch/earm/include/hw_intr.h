#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(ཧྲིལ irq);

void hw_intr_mask(ཧྲིལ irq);
void hw_intr_unmask(ཧྲིལ irq);
void hw_intr_ack(ཧྲིལ irq);
void hw_intr_used(ཧྲིལ irq);
void hw_intr_not_used(ཧྲིལ irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
