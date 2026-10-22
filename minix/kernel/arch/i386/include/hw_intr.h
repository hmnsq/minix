#अगर_अघोषित __HW_INTR_X86_H__
#घोषणा __HW_INTR_X86_H__

#समावेश "kernel/kernel.h"
व्योम irq_8259_unmask(पूर्णांक irq);
व्योम irq_8259_mask(पूर्णांक irq);
व्योम irq_8259_eoi(पूर्णांक irq);
व्योम irq_handle(पूर्णांक irq);
व्योम i8259_disable(व्योम);
व्योम eoi_8259_master(व्योम);
व्योम eoi_8259_slave(व्योम);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#अगर defined(USE_APIC)
#समावेश "kernel/arch/i386/apic.h"

#घोषणा hw_intr_mask(irq)	ioapic_mask_irq(irq)
#घोषणा hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#घोषणा hw_intr_ack(irq)	ioapic_eoi(irq)
#घोषणा hw_intr_used(irq)	करो {					\
					अगर (ioapic_enabled)		\
						ioapic_set_irq(irq);	\
				} जबतक (0)
#घोषणा hw_intr_not_used(irq)	करो {					\
					अगर (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} जबतक (0)
#घोषणा hw_intr_disable_all() करो {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} जबतक (0)
#अगर_घोषित CONFIG_SMP
#घोषणा ipi_ack			apic_eoi
#पूर्ण_अगर

#अन्यथा
/* legacy PIC */

#घोषणा hw_intr_mask(irq)	irq_8259_mask(irq)
#घोषणा hw_intr_unmask(irq)	irq_8259_unmask(irq)
#घोषणा hw_intr_ack(irq)	irq_8259_eoi(irq)
#घोषणा hw_intr_used(irq)
#घोषणा hw_intr_not_used(irq)
#घोषणा hw_intr_disable_all()

#पूर्ण_अगर

#पूर्ण_अगर /* __HW_INTR_X86_H__ */
