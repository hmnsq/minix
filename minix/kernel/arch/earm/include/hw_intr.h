#اگر_تعریف_نشده __HW_INTR_ARM_H__
#تعریف_کن __HW_INTR_ARM_H__


#ضمن "kernel/kernel.h"
فراغ irq_handle(صحيح irq);

فراغ hw_intr_ماسک(صحيح irq);
فراغ hw_intr_unmask(صحيح irq);
فراغ hw_intr_ack(صحيح irq);
فراغ hw_intr_استفاده_شده(صحيح irq);
فراغ hw_intr_not_استفاده_شده(صحيح irq);
فراغ hw_intr_disable_all(فراغ);

#پایان_اگر /* __HW_INTR_ARM_H__ */
