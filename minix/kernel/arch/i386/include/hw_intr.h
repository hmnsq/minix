#اگر_غیر_تعریف_شدہ __HW_INTR_X86_H__
#تعریف_کرو __HW_INTR_X86_H__

#شامل "kernel/kernel.h"
خالی irq_8259_unmask(صحیح irq);
خالی irq_8259_نقاب(صحیح irq);
خالی irq_8259_eoi(صحیح irq);
خالی irq_handle(صحیح irq);
خالی i8259_disable(خالی);
خالی eoi_8259_master(خالی);
خالی eoi_8259_slave(خالی);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#اگر defined(USE_APIC)
#شامل "kernel/arch/i386/apic.h"

#تعریف_کرو hw_intr_نقاب(irq)	ioapic_نقاب_irq(irq)
#تعریف_کرو hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#تعریف_کرو hw_intr_ack(irq)	ioapic_eoi(irq)
#تعریف_کرو hw_intr_مستعمل(irq)	کرو {					\
					اگر (ioapic_enabled)		\
						ioapic_سیٹ_irq(irq);	\
				} جب (0)
#تعریف_کرو hw_intr_not_مستعمل(irq)	کرو {					\
					اگر (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} جب (0)
#تعریف_کرو hw_intr_disable_all() کرو {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} جب (0)
#اگر_تعریف_شدہ CONFIG_SMP
#تعریف_کرو ipi_ack			apic_eoi
#اگر_ختم

#ورنہ
/* legacy PIC */

#تعریف_کرو hw_intr_نقاب(irq)	irq_8259_نقاب(irq)
#تعریف_کرو hw_intr_unmask(irq)	irq_8259_unmask(irq)
#تعریف_کرو hw_intr_ack(irq)	irq_8259_eoi(irq)
#تعریف_کرو hw_intr_مستعمل(irq)
#تعریف_کرو hw_intr_not_مستعمل(irq)
#تعریف_کرو hw_intr_disable_all()

#اگر_ختم

#اگر_ختم /* __HW_INTR_X86_H__ */
