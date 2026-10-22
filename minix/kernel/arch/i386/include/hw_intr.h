#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#شامل "kernel/kernel.h"
تهی irq_8259_unmask(صحیح irq);
تهی irq_8259_mask(صحیح irq);
تهی irq_8259_eoi(صحیح irq);
تهی irq_handle(صحیح irq);
تهی i8259_disable(تهی);
تهی eoi_8259_master(تهی);
تهی eoi_8259_slave(تهی);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#اگر defined(USE_APIC)
#شامل "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	انجامبده {					\
					اگر (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} وقتی (0)
#define hw_intr_not_used(irq)	انجامبده {					\
					اگر (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} وقتی (0)
#define hw_intr_disable_all() انجامبده {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} وقتی (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#وگرنه
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
