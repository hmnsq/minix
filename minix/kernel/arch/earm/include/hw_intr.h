#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#অন্তর্ভাব "kernel/kernel.h"
শূন্য irq_handle(পূর্ণসংখ্যা irq);

শূন্য hw_intr_mask(পূর্ণসংখ্যা irq);
শূন্য hw_intr_unmask(পূর্ণসংখ্যা irq);
শূন্য hw_intr_ack(পূর্ণসংখ্যা irq);
শূন্য hw_intr_used(পূর্ণসংখ্যা irq);
শূন্য hw_intr_not_used(পূর্ণসংখ্যা irq);
শূন্য hw_intr_disable_all(শূন্য);

#endif /* __HW_INTR_ARM_H__ */
