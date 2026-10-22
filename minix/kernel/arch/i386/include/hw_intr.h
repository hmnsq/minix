#که_نه_تعریف_شوی __HW_INTR_X86_H__
#تعریف_کړه __HW_INTR_X86_H__

#ضمن "kernel/kernel.h"
فراغ irq_8259_unmask(صحيح irq);
فراغ irq_8259_ماسک(صحيح irq);
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

#تعریف_کړه hw_intr_ماسک(irq)	ioapic_ماسک_irq(irq)
#تعریف_کړه hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#تعریف_کړه hw_intr_ack(irq)	ioapic_eoi(irq)
#تعریف_کړه hw_intr_کارول_شوی(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_سېټ_irq(irq);	\
				} طالما (0)
#تعریف_کړه hw_intr_not_کارول_شوی(irq)	افعل {					\
					إذا (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} طالما (0)
#تعریف_کړه hw_intr_disable_all() افعل {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} طالما (0)
#که_تعریف_شوی CONFIG_SMP
#تعریف_کړه ipi_ack			apic_eoi
#د_که_پای

#وإلا
/* legacy PIC */

#تعریف_کړه hw_intr_ماسک(irq)	irq_8259_ماسک(irq)
#تعریف_کړه hw_intr_unmask(irq)	irq_8259_unmask(irq)
#تعریف_کړه hw_intr_ack(irq)	irq_8259_eoi(irq)
#تعریف_کړه hw_intr_کارول_شوی(irq)
#تعریف_کړه hw_intr_not_کارول_شوی(irq)
#تعریف_کړه hw_intr_disable_all()

#د_که_پای

#د_که_پای /* __HW_INTR_X86_H__ */
