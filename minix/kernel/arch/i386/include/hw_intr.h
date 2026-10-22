#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#അന്തര്ഭാവ "kernel/kernel.h"
ശൂന്യം irq_8259_unmask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം irq_8259_mask(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം irq_8259_eoi(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം irq_handle(പൂർണ്ണസംഖ്യ irq);
ശൂന്യം i8259_disable(ശൂന്യം);
ശൂന്യം eoi_8259_master(ശൂന്യം);
ശൂന്യം eoi_8259_slave(ശൂന്യം);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#എങ്കിൽ defined(USE_APIC)
#അന്തര്ഭാവ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	ചെയ്യുക {					\
					എങ്കിൽ (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} ആകുമ്പോൾ (0)
#define hw_intr_not_used(irq)	ചെയ്യുക {					\
					എങ്കിൽ (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ആകുമ്പോൾ (0)
#define hw_intr_disable_all() ചെയ്യുക {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ആകുമ്പോൾ (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#അല്ലെങ്കിൽ
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
