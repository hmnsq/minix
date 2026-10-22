#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#અન્તર્ભાવ "kernel/kernel.h"
શૂન્ય irq_8259_unmask(પૂર્ણાંક irq);
શૂન્ય irq_8259_mask(પૂર્ણાંક irq);
શૂન્ય irq_8259_eoi(પૂર્ણાંક irq);
શૂન્ય irq_handle(પૂર્ણાંક irq);
શૂન્ય i8259_disable(શૂન્ય);
શૂન્ય eoi_8259_master(શૂન્ય);
શૂન્ય eoi_8259_slave(શૂન્ય);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#જો defined(USE_APIC)
#અન્તર્ભાવ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	કરો {					\
					જો (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} જ્યારે (0)
#define hw_intr_not_used(irq)	કરો {					\
					જો (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} જ્યારે (0)
#define hw_intr_disable_all() કરો {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} જ્યારે (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#નહીંતર
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
