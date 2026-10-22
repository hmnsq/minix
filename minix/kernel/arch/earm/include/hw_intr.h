#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#अन्तर्भाव "kernel/kernel.h"
शून्य irq_handle(पूर्णांक irq);

शून्य hw_intr_mask(पूर्णांक irq);
शून्य hw_intr_unmask(पूर्णांक irq);
शून्य hw_intr_ack(पूर्णांक irq);
शून्य hw_intr_used(पूर्णांक irq);
शून्य hw_intr_not_used(पूर्णांक irq);
शून्य hw_intr_disable_all(शून्य);

#endif /* __HW_INTR_ARM_H__ */
