#جيڪڏهن_وصف_نه_ٿيل __HW_INTR_X86_H__
#وصف_ڪريو __HW_INTR_X86_H__

#ضمن "kernel/kernel.h"
فراغ irq_8259_unmask(صحيح irq);
فراغ irq_8259_نقاب(صحيح irq);
فراغ irq_8259_eoi(صحيح irq);
فراغ irq_handle(صحيح irq);
فراغ i8259_disable(فراغ);
فراغ eoi_8259_master(فراغ);
فراغ eoi_8259_slave(فراغ);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#إذا defined(USE_APIC)
#ضمن "kernel/arch/i386/apic.h"

#وصف_ڪريو hw_intr_نقاب(irq)	ioapic_نقاب_irq(irq)
#وصف_ڪريو hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#وصف_ڪريو hw_intr_ack(irq)	ioapic_eoi(irq)
#وصف_ڪريو hw_intr_استعمال_ٿيل(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_سيٽ_irq(irq);	\
				} طالما (0)
#وصف_ڪريو hw_intr_not_استعمال_ٿيل(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} طالما (0)
#وصف_ڪريو hw_intr_disable_all() افعل {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} طالما (0)
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
#وصف_ڪريو ipi_ack			apic_eoi
#جيڪڏهن_پورو

#وإلا
/* legacy PIC */

#وصف_ڪريو hw_intr_نقاب(irq)	irq_8259_نقاب(irq)
#وصف_ڪريو hw_intr_unmask(irq)	irq_8259_unmask(irq)
#وصف_ڪريو hw_intr_ack(irq)	irq_8259_eoi(irq)
#وصف_ڪريو hw_intr_استعمال_ٿيل(irq)
#وصف_ڪريو hw_intr_not_استعمال_ٿيل(irq)
#وصف_ڪريو hw_intr_disable_all()

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* __HW_INTR_X86_H__ */
