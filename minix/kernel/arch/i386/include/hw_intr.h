#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __HW_INTR_X86_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __HW_INTR_X86_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
ਖਾਲੀ irq_8259_unmask(ਪੂਰਨ irq);
ਖਾਲੀ irq_8259_ਨਕਾਬ(ਪੂਰਨ irq);
ਖਾਲੀ irq_8259_eoi(ਪੂਰਨ irq);
ਖਾਲੀ irq_handle(ਪੂਰਨ irq);
ਖਾਲੀ i8259_disable(ਖਾਲੀ);
ਖਾਲੀ eoi_8259_master(ਖਾਲੀ);
ਖਾਲੀ eoi_8259_slave(ਖਾਲੀ);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ਜੇ defined(USE_APIC)
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/arch/i386/apic.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ਨਕਾਬ(irq)	ioapic_ਨਕਾਬ_irq(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ack(irq)	ioapic_eoi(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ਵਰਤਿਆ(irq)	ਕਰੋ {					\
					ਜੇ (ioapic_enabled)		\
						ioapic_ਸੈੱਟ_irq(irq);	\
				} ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_not_ਵਰਤਿਆ(irq)	ਕਰੋ {					\
					ਜੇ (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_disable_all() ਕਰੋ {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ਜਦੋਂ (0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ipi_ack			apic_eoi
#ਜੇ_ਅੰਤ

#ਨਹੀਂਤਾਂ
/* legacy PIC */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ਨਕਾਬ(irq)	irq_8259_ਨਕਾਬ(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_unmask(irq)	irq_8259_unmask(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ack(irq)	irq_8259_eoi(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_ਵਰਤਿਆ(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_not_ਵਰਤਿਆ(irq)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hw_intr_disable_all()

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* __HW_INTR_X86_H__ */
