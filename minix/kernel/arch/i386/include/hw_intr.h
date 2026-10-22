#यदि_अपरिभाषितम् __HW_INTR_X86_H__
#परिभाषय __HW_INTR_X86_H__

#अन्तर्भाव "kernel/kernel.h"
शून्य irq_8259_unmask(पूर्णाङ्क irq);
शून्य irq_8259_आवरणम्(पूर्णाङ्क irq);
शून्य irq_8259_eoi(पूर्णाङ्क irq);
शून्य irq_handle(पूर्णाङ्क irq);
शून्य i8259_disable(शून्य);
शून्य eoi_8259_master(शून्य);
शून्य eoi_8259_slave(शून्य);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#यदि defined(USE_APIC)
#अन्तर्भाव "kernel/arch/i386/apic.h"

#परिभाषय hw_intr_आवरणम्(irq)	ioapic_आवरणम्_irq(irq)
#परिभाषय hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#परिभाषय hw_intr_ack(irq)	ioapic_eoi(irq)
#परिभाषय hw_intr_प्रयुक्तम्(irq)	कुरु {					\
					यदि (ioapic_enabled)		\
						ioapic_समुच्चयः_irq(irq);	\
				} यावत् (0)
#परिभाषय hw_intr_not_प्रयुक्तम्(irq)	कुरु {					\
					यदि (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} यावत् (0)
#परिभाषय hw_intr_disable_all() कुरु {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} यावत् (0)
#यदि_परिभाषितम् CONFIG_SMP
#परिभाषय ipi_ack			apic_eoi
#यदि_अन्तः

#अन्यथा
/* legacy PIC */

#परिभाषय hw_intr_आवरणम्(irq)	irq_8259_आवरणम्(irq)
#परिभाषय hw_intr_unmask(irq)	irq_8259_unmask(irq)
#परिभाषय hw_intr_ack(irq)	irq_8259_eoi(irq)
#परिभाषय hw_intr_प्रयुक्तम्(irq)
#परिभाषय hw_intr_not_प्रयुक्तम्(irq)
#परिभाषय hw_intr_disable_all()

#यदि_अन्तः

#यदि_अन्तः /* __HW_INTR_X86_H__ */
