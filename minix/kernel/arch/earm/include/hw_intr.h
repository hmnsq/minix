#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#include "kernel/kernel.h"
void irq_handle(ຈຳນວນເຕັມ irq);

void hw_intr_mask(ຈຳນວນເຕັມ irq);
void hw_intr_unmask(ຈຳນວນເຕັມ irq);
void hw_intr_ack(ຈຳນວນເຕັມ irq);
void hw_intr_used(ຈຳນວນເຕັມ irq);
void hw_intr_not_used(ຈຳນວນເຕັມ irq);
void hw_intr_disable_all(void);

#endif /* __HW_INTR_ARM_H__ */
