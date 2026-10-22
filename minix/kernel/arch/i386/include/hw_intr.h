#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
ಶೂನ್ಯ irq_8259_unmask(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ irq_8259_mask(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ irq_8259_eoi(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ irq_handle(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ i8259_disable(ಶೂನ್ಯ);
ಶೂನ್ಯ eoi_8259_master(ಶೂನ್ಯ);
ಶೂನ್ಯ eoi_8259_slave(ಶೂನ್ಯ);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ಯದಿ defined(USE_APIC)
#ಅನ್ತರ್ಭಾವ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	ಕುರು {					\
					ಯದಿ (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} ಯಾವತ್ (0)
#define hw_intr_not_used(irq)	ಕುರು {					\
					ಯದಿ (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ಯಾವತ್ (0)
#define hw_intr_disable_all() ಕುರು {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ಯಾವತ್ (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#ಅನ್ಯಥಾ
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
