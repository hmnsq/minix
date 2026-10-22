#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#ضمن "kernel/kernel.h"
فراغ irq_handle(صحيح irq);

فراغ hw_intr_mask(صحيح irq);
فراغ hw_intr_unmask(صحيح irq);
فراغ hw_intr_ack(صحيح irq);
فراغ hw_intr_used(صحيح irq);
فراغ hw_intr_not_used(صحيح irq);
فراغ hw_intr_disable_all(فراغ);

#endif /* __HW_INTR_ARM_H__ */
