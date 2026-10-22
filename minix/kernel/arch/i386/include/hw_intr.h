#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __HW_INTR_X86_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __HW_INTR_X86_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
ಶೂನ್ಯ irq_8259_unmask(ಪೂರ್ಣಾಂಕ irq);
ಶೂನ್ಯ irq_8259_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ irq);
ಶೂನ್ಯ irq_8259_eoi(ಪೂರ್ಣಾಂಕ irq);
ಶೂನ್ಯ irq_handle(ಪೂರ್ಣಾಂಕ irq);
ಶೂನ್ಯ i8259_disable(ಶೂನ್ಯ);
ಶೂನ್ಯ eoi_8259_master(ಶೂನ್ಯ);
ಶೂನ್ಯ eoi_8259_slave(ಶೂನ್ಯ);

/* 
 * we don't use IO APIC if not configured for SMP as we cannot read any info
 * about it unless we use MPS which is not present on all single CPU
 * configurations. ACPI would be another option, however we don't support it
 * either
 */
#ಆದರೆ defined(USE_APIC)
#ಅನ್ತರ್ಭಾವ "kernel/arch/i386/apic.h"

#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ಮುಖವಾಡ(irq)	ioapic_ಮುಖವಾಡ_irq(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_unmask(irq)	ioapic_unmask_irq(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ack(irq)	ioapic_eoi(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ಬಳಸಿದ(irq)	ಮಾಡು {					\
					ಆದರೆ (ioapic_enabled)		\
						ioapic_ಗಣ_irq(irq);	\
				} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_not_ಬಳಸಿದ(irq)	ಮಾಡು {					\
					ಆದರೆ (ioapic_enabled)		\
						ioapic_unset_irq(irq);	\
				} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_disable_all() ಮಾಡು {					\
					ioapic_disable_all();		\
					ioapic_reset_pic();		\
					lapic_disable();		\
				} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
#ವ್ಯಾಖ್ಯಾನಿಸು ipi_ack			apic_eoi
#ಆದರೆ_ಅಂತ್ಯ

#ಇಲ್ಲದಿದ್ದರೆ
/* legacy PIC */

#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ಮುಖವಾಡ(irq)	irq_8259_ಮುಖವಾಡ(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_unmask(irq)	irq_8259_unmask(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ack(irq)	irq_8259_eoi(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_ಬಳಸಿದ(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_not_ಬಳಸಿದ(irq)
#ವ್ಯಾಖ್ಯಾನಿಸು hw_intr_disable_all()

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* __HW_INTR_X86_H__ */
