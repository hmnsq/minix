#جے_بنا_تعریف __HW_INTR_X86_H__
#تعریف_کرو __HW_INTR_X86_H__

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

#تعریف_کرو hw_intr_نقاب(irq)	ioapic_نقاب_irq(irq)
#تعریف_کرو hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#تعریف_کرو hw_intr_ack(irq)	ioapic_eoi(irq)
#تعریف_کرو hw_intr_ورتیا(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_سیٹ_irq(irq);	\
				} طالما (0)
#تعریف_کرو hw_intr_not_ورتیا(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} طالما (0)
#تعریف_کرو hw_intr_disable_all() افعل {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} طالما (0)
#جے_تعریف_شدہ CONFIG_SMP
#تعریف_کرو ipi_ack			apic_eoi
#جے_انت

#وإلا
/* legacy PIC */

#تعریف_کرو hw_intr_نقاب(irq)	irq_8259_نقاب(irq)
#تعریف_کرو hw_intr_unmask(irq)	irq_8259_unmask(irq)
#تعریف_کرو hw_intr_ack(irq)	irq_8259_eoi(irq)
#تعریف_کرو hw_intr_ورتیا(irq)
#تعریف_کرو hw_intr_not_ورتیا(irq)
#تعریف_کرو hw_intr_disable_all()

#جے_انت

#جے_انت /* __HW_INTR_X86_H__ */
