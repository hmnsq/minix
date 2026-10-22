#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#অন্তর্ভাৱ "kernel/kernel.h"
শূন্য irq_8259_unmask(পূর্ণাঙ্ক irq);
শূন্য irq_8259_mask(পূর্ণাঙ্ক irq);
শূন্য irq_8259_eoi(পূর্ণাঙ্ক irq);
শূন্য irq_handle(পূর্ণাঙ্ক irq);
শূন্য i8259_disable(শূন্য);
শূন্য eoi_8259_master(শূন্য);
শূন্য eoi_8259_slave(শূন্য);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#যদি defined(USE_APIC)
#অন্তর্ভাৱ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	কৰা {					\
					যদি (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} যেতিয়ালৈ (0)
#define hw_intr_not_used(irq)	কৰা {					\
					যদি (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} যেতিয়ালৈ (0)
#define hw_intr_disable_all() কৰা {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} যেতিয়ালৈ (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#অন্যথা
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
