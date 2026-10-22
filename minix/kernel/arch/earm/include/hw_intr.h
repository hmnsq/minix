#که_نه_تعریف_شوی __HW_INTR_ARM_H__
#تعریف_کړه __HW_INTR_ARM_H__


#ضمن "kernel/kernel.h"
فراغ irq_handle(صحيح irq);

فراغ hw_intr_ماسک(صحيح irq);
فراغ hw_intr_unmask(صحيح irq);
فراغ hw_intr_ack(صحيح irq);
فراغ hw_intr_کارول_شوی(صحيح irq);
فراغ hw_intr_not_کارول_شوی(صحيح irq);
فراغ hw_intr_disable_all(فراغ);

#د_که_پای /* __HW_INTR_ARM_H__ */
