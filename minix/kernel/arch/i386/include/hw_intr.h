#నిర్వచితం_కాకపోతే __HW_INTR_X86_H__
#నిర్వచించు __HW_INTR_X86_H__

#అన్తర్భావ "kernel/kernel.h"
శూన్య irq_8259_unmask(పూర్ణాంకం irq);
శూన్య irq_8259_ముసుగు(పూర్ణాంకం irq);
శూన్య irq_8259_eoi(పూర్ణాంకం irq);
శూన్య irq_handle(పూర్ణాంకం irq);
శూన్య i8259_disable(శూన్య);
శూన్య eoi_8259_master(శూన్య);
శూన్య eoi_8259_slave(శూన్య);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ఒకవేళ defined(USE_APIC)
#అన్తర్భావ "kernel/arch/i386/apic.h"

#నిర్వచించు hw_intr_ముసుగు(irq)	ioapic_ముసుగు_irq(irq)
#నిర్వచించు hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#నిర్వచించు hw_intr_ack(irq)	ioapic_eoi(irq)
#నిర్వచించు hw_intr_వాడిన(irq)	చేయి {					\
					ఒకవేళ (ioapic_enabled)		\
						ioapic_సమితి_irq(irq);	\
				} అయితే (0)
#నిర్వచించు hw_intr_not_వాడిన(irq)	చేయి {					\
					ఒకవేళ (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} అయితే (0)
#నిర్వచించు hw_intr_disable_all() చేయి {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} అయితే (0)
#నిర్వచితమైతే CONFIG_SMP
#నిర్వచించు ipi_ack			apic_eoi
#అయితే_ముగింపు

#లేకపోతే
/* legacy PIC */

#నిర్వచించు hw_intr_ముసుగు(irq)	irq_8259_ముసుగు(irq)
#నిర్వచించు hw_intr_unmask(irq)	irq_8259_unmask(irq)
#నిర్వచించు hw_intr_ack(irq)	irq_8259_eoi(irq)
#నిర్వచించు hw_intr_వాడిన(irq)
#నిర్వచించు hw_intr_not_వాడిన(irq)
#నిర్వచించు hw_intr_disable_all()

#అయితే_ముగింపు

#అయితే_ముగింపు /* __HW_INTR_X86_H__ */
