#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#अन्तर्भाव "kernel/kernel.h"
शून्य irq_8259_unmask(पूर्णाङ्क irq);
शून्य irq_8259_mask(पूर्णाङ्क irq);
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
#सचे defined(USE_APIC)
#अन्तर्भाव "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	करोहि {					\
					सचे (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} याव (0)
#define hw_intr_not_used(irq)	करोहि {					\
					सचे (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} याव (0)
#define hw_intr_disable_all() करोहि {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} याव (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#अञ्ञथा
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
