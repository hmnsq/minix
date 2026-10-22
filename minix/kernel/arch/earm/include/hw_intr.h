#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#અન્તર્ભાવ "kernel/kernel.h"
શૂન્ય irq_handle(પૂર્ણાંક irq);

શૂન્ય hw_intr_mask(પૂર્ણાંક irq);
શૂન્ય hw_intr_unmask(પૂર્ણાંક irq);
શૂન્ય hw_intr_ack(પૂર્ણાંક irq);
શૂન્ય hw_intr_used(પૂર્ણાંક irq);
શૂન્ય hw_intr_not_used(પૂર્ણાંક irq);
શૂન્ય hw_intr_disable_all(શૂન્ય);

#endif /* __HW_INTR_ARM_H__ */
