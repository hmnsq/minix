#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#شامل "kernel/kernel.h"
تهی irq_handle(صحیح irq);

تهی hw_intr_mask(صحیح irq);
تهی hw_intr_unmask(صحیح irq);
تهی hw_intr_ack(صحیح irq);
تهی hw_intr_used(صحیح irq);
تهی hw_intr_not_used(صحیح irq);
تهی hw_intr_disable_all(تهی);

#endif /* __HW_INTR_ARM_H__ */
