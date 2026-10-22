#यदि_अपरिभाषितम् __HW_INTR_ARM_H__
#परिभाषय __HW_INTR_ARM_H__


#अन्तर्भाव "kernel/kernel.h"
शून्य irq_handle(पूर्णाङ्क irq);

शून्य hw_intr_आवरणम्(पूर्णाङ्क irq);
शून्य hw_intr_unmask(पूर्णाङ्क irq);
शून्य hw_intr_ack(पूर्णाङ्क irq);
शून्य hw_intr_प्रयुक्तम्(पूर्णाङ्क irq);
शून्य hw_intr_not_प्रयुक्तम्(पूर्णाङ्क irq);
शून्य hw_intr_disable_all(शून्य);

#यदि_अन्तः /* __HW_INTR_ARM_H__ */
