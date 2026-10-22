#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
ଶୂନ୍ୟ irq_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);

ଶୂନ୍ୟ hw_intr_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ hw_intr_unmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ hw_intr_ack(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ hw_intr_used(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ hw_intr_not_used(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ hw_intr_disable_all(ଶୂନ୍ୟ);

#endif /* __HW_INTR_ARM_H__ */
