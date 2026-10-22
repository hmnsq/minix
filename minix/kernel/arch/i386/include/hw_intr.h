#ifndef __HW_INTR_X86_H__
#define __HW_INTR_X86_H__

#include "kernel/kernel.h"
void irq_8259_unmask(จำนวนเต็ม irq);
void irq_8259_mask(จำนวนเต็ม irq);
void irq_8259_eoi(จำนวนเต็ม irq);
void irq_handle(จำนวนเต็ม irq);
void i8259_disable(void);
void eoi_8259_master(void);
void eoi_8259_slave(void);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ถ้า defined(USE_APIC)
#include "kernel/arch/i386/apic.h"

#define hw_intr_mask(irq)	ioapic_mask_irq(irq)
#define hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#define hw_intr_ack(irq)	ioapic_eoi(irq)
#define hw_intr_used(irq)	do {					\
					ถ้า (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} ขณะ (0)
#define hw_intr_not_used(irq)	do {					\
					ถ้า (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ขณะ (0)
#define hw_intr_disable_all() do {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ขณะ (0)
#ifdef CONFIG_SMP
#define ipi_ack			apic_eoi
#endif

#มิฉะนั้น
/* legacy PIC */

#define hw_intr_mask(irq)	irq_8259_mask(irq)
#define hw_intr_unmask(irq)	irq_8259_unmask(irq)
#define hw_intr_ack(irq)	irq_8259_eoi(irq)
#define hw_intr_used(irq)
#define hw_intr_not_used(irq)
#define hw_intr_disable_all()

#endif

#endif /* __HW_INTR_X86_H__ */
