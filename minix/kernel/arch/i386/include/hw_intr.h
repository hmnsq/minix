#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#அந்தர்பாவ "kernel/kernel.h"
வெறுமை irq_8259_unmask(முழு irq);
வெறுமை irq_8259_mask(முழு irq);
வெறுமை irq_8259_eoi(முழு irq);
வெறுமை irq_handle(முழு irq);
வெறுமை i8259_disable(வெறுமை);
வெறுமை eoi_8259_master(வெறுமை);
வெறுமை eoi_8259_slave(வெறுமை);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#எனில் defined(USE_APIC)
#அந்தர்பாவ "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	செய் {					\
					எனில் (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} போது (0)
#define hw_intr_not_used(irq)	செய் {					\
					எனில் (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} போது (0)
#define hw_intr_disable_all() செய் {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} போது (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#இல்லையெனில்
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
