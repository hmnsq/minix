#अगर_अघोषित __HW_INTR_ARM_H__
#घोषणा __HW_INTR_ARM_H__


#समावेश "kernel/kernel.h"
व्योम irq_handle(पूर्णांक irq);

व्योम hw_intr_mask(पूर्णांक irq);
व्योम hw_intr_unmask(पूर्णांक irq);
व्योम hw_intr_ack(पूर्णांक irq);
व्योम hw_intr_used(पूर्णांक irq);
व्योम hw_intr_not_used(पूर्णांक irq);
व्योम hw_intr_disable_all(व्योम);

#पूर्ण_अगर /* __HW_INTR_ARM_H__ */
