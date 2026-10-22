#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
ଶୂନ୍ୟ irq_8259_unmask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ irq_8259_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ irq_8259_eoi(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ irq_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ଶୂନ୍ୟ i8259_disable(ଶୂନ୍ୟ);
ଶୂନ୍ୟ eoi_8259_master(ଶୂନ୍ୟ);
ଶୂନ୍ୟ eoi_8259_slave(ଶୂନ୍ୟ);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ଯଦି defined(USE_APIC)
#ଅନ୍ତର୍ଭାଵ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	କର {					\
					ଯଦି (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} ଯେତେବେଳେ (0)
#define hw_intr_not_used(irq)	କର {					\
					ଯଦି (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ଯେତେବେଳେ (0)
#define hw_intr_disable_all() କର {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ଯେତେବେଳେ (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#ଅନ୍ୟଥା
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
