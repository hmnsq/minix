#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#அந்தர்பாவ "kernel/kernel.h"
வெறுமை irq_handle(முழு irq);

வெறுமை hw_intr_mask(முழு irq);
வெறுமை hw_intr_unmask(முழு irq);
வெறுமை hw_intr_ack(முழு irq);
வெறுமை hw_intr_used(முழு irq);
வெறுமை hw_intr_not_used(முழு irq);
வெறுமை hw_intr_disable_all(வெறுமை);

#endif /* __HW_INTR_ARM_H__ */
