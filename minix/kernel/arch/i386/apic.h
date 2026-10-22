#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __APIC_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __APIC_H__

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ENABLE		0x100
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_FOCUS_DISABLED	(1 << 9)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_SIV		0xFF

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_2	0x00
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_4	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_8	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_16	0x03
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_32	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_64	0x09
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_128	0x0a
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_1	0x0b

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_ಸದಿಶ_ಮುಖವಾಡ	0x000000FF
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_DS_PENDING	(1 << 12)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_ಮುಖವಾಡ		(1 << 16)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_TM		(1 << 17)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_ಮುಖವಾಡ	0x00002000
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_AH	0x00002000
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_AL	0x00000000

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REGSEL		0x0
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_RW		0x10

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_ಮುಖವಾಡ		0x00000700
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_ಸದಿಶ			APIC_LVTT_ಸದಿಶ_ಮುಖವಾಡ
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_FIXED		(0 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_ಅತಿಕಡಿಮೆ_ಆದ್ಯತೆ	(1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_SMI			(2 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_RESERVED		(3 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_NMI			(4 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_INIT		(5 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_STARTUP		(6 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_EXTINT		(7 << 8)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_PHYSICAL		(0 << 11)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_LOGICAL		(1 << 11)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DELIVERY_PENDING	(1 << 12)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INT_POLARITY		(1 << 13)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_LEVEL_ASSERT		(1 << 14)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_TRIGGER		(1 << 15)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INT_ಮುಖವಾಡ		(1 << 16)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ಕ್ಷೇತ್ರ		(0 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_SELF		(1 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ALL		(2 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#ವ್ಯಾಖ್ಯಾನಿಸು LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#ವ್ಯಾಖ್ಯಾನಿಸು IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ID	(lapic_addr + 0x020)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ಆವೃತ್ತಿ	(lapic_addr + 0x030)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_TPR	(lapic_addr + 0x080)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_EOI	(lapic_addr + 0x0b0)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LDR	(lapic_addr + 0x0d0)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_DFR	(lapic_addr + 0x0e0)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_SIVR	(lapic_addr + 0x0f0)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ISR	(lapic_addr + 0x100)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_TMR	(lapic_addr + 0x180)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_IRR	(lapic_addr + 0x200)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ESR	(lapic_addr + 0x280)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ICR1	(lapic_addr + 0x300)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ICR2	(lapic_addr + 0x310)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LVTTR	(lapic_addr + 0x320)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LVTTMR	(lapic_addr + 0x330)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LVTPCR	(lapic_addr + 0x340)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LINT0	(lapic_addr + 0x350)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LINT1	(lapic_addr + 0x360)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_LVTER	(lapic_addr + 0x370)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_ID			0x0
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_ಆವೃತ್ತಿ			0x1
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_ARB			0x2
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REDIR_ಕೋಷ್ಟಕ		0x10

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TIMER_INT_ಸದಿಶ		0xf0
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_SMP_SCHED_PROC_ಸದಿಶ	0xf1
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_SMP_CPU_HALT_ಸದಿಶ	0xf2
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ERROR_INT_ಸದಿಶ		0xfe
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_SPURIOUS_INT_ಸದಿಶ	0xff

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

EXTERN vir_ಬೈಟುಗಳು lapic_addr;
EXTERN vir_ಬೈಟುಗಳು lapic_eoi_addr;
EXTERN ಪೂರ್ಣಾಂಕ ioapic_enabled;
EXTERN ಪೂರ್ಣಾಂಕ bsp_lapic_id;

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NR_IOAPICS		32
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_IOAPIC_IRQS		64

EXTERN ಪೂರ್ಣಾಂಕ ioapic_enabled;

ರಚನೆ io_apic {
	ಚಿಹ್ನೆರಹಿತ	id;
	vir_ಬೈಟುಗಳು	addr; /* presently used address */
	phys_ಬೈಟುಗಳು	paddr; /* where is it in phys space */
	vir_ಬೈಟುಗಳು	vaddr; /* address after paging is on */
	ಚಿಹ್ನೆರಹಿತ	pins;
	ಚಿಹ್ನೆರಹಿತ	gsi_ತಳ;
};

EXTERN ರಚನೆ io_apic io_apic[MAX_NR_IOAPICS];
EXTERN ಚಿಹ್ನೆರಹಿತ nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

ಪೂರ್ಣಾಂಕ lapic_enable(ಚಿಹ್ನೆರಹಿತ cpu);
ಶೂನ್ಯ ioapic_unmask_irq(ಚಿಹ್ನೆರಹಿತ irq);
ಶೂನ್ಯ ioapic_ಮುಖವಾಡ_irq(ಚಿಹ್ನೆರಹಿತ irq);
ಶೂನ್ಯ ioapic_reset_pic(ಶೂನ್ಯ);

EXTERN ಪೂರ್ಣಾಂಕ ioapic_enabled;
EXTERN ಚಿಹ್ನೆರಹಿತ nioapics;

ಶೂನ್ಯ lapic_microsec_sleep(ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2);
ಶೂನ್ಯ ioapic_disable_irqs(u32_t irqs);
ಶೂನ್ಯ ioapic_enable_irqs(u32_t irqs);

ಪೂರ್ಣಾಂಕ lapic_enable(ಚಿಹ್ನೆರಹಿತ cpu);
ಶೂನ್ಯ lapic_disable(ಶೂನ್ಯ);

ಶೂನ್ಯ ioapic_disable_all(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ ioapic_enable_all(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ detect_ioapics(ಶೂನ್ಯ);
ಶೂನ್ಯ apic_idt_init(ಪೂರ್ಣಾಂಕ reset);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಪೂರ್ಣಾಂಕ apic_send_startup_ipi(ಚಿಹ್ನೆರಹಿತ cpu, phys_ಬೈಟುಗಳು trampoline);
ಪೂರ್ಣಾಂಕ apic_send_init_ipi(ಚಿಹ್ನೆರಹಿತ cpu, phys_ಬೈಟುಗಳು trampoline);
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ apicid(ಶೂನ್ಯ);
ಶೂನ್ಯ ioapic_ಗಣ_id(u32_t addr, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ id);
#ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ apic_single_cpu_init(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ lapic_ಗಣ_timer_periodic(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq);
ಶೂನ್ಯ lapic_ಗಣ_timer_one_shot(ಸ್ಥಿರಾಂಕ u32_t ಮೌಲ್ಯ);
ಶೂನ್ಯ lapic_stop_timer(ಶೂನ್ಯ);
ಶೂನ್ಯ lapic_restart_timer(ಶೂನ್ಯ);

ಶೂನ್ಯ ioapic_ಗಣ_irq(ಚಿಹ್ನೆರಹಿತ irq);
ಶೂನ್ಯ ioapic_unset_irq(ಚಿಹ್ನೆರಹಿತ irq);

/* signal the end of interrupt handler to apic */
#ವ್ಯಾಖ್ಯಾನಿಸು apic_eoi() ಮಾಡು { *((ಅಸ್ಥಿರ u32_t *) lapic_eoi_addr) = 0; } ಆಗ(0)

ಶೂನ್ಯ ioapic_eoi(ಪೂರ್ಣಾಂಕ irq);

ಶೂನ್ಯ dump_apic_irq_state(ಶೂನ್ಯ);

ಶೂನ್ಯ apic_send_ipi(ಚಿಹ್ನೆರಹಿತ ಸದಿಶ, ಚಿಹ್ನೆರಹಿತ cpu, ಪೂರ್ಣಾಂಕ type);

ಶೂನ್ಯ apic_ipi_sched_intr(ಶೂನ್ಯ);
ಶೂನ್ಯ apic_ipi_halt_intr(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_IPI_DEST			0
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_IPI_SELF			1
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_IPI_ಗಮ್ಯ_ALL			2
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_IPI_ಗಮ್ಯ_ALL_BUT_SELF	3

#ವ್ಯಾಖ್ಯಾನಿಸು apic_send_ipi_single(ಸದಿಶ,cpu) \
	apic_send_ipi(ಸದಿಶ, cpu, APIC_IPI_DEST);
#ವ್ಯಾಖ್ಯಾನಿಸು apic_send_ipi_self(ಸದಿಶ) \
	apic_send_ipi(ಸದಿಶ, 0, APIC_IPI_SELF)
#ವ್ಯಾಖ್ಯಾನಿಸು apic_send_ipi_all(ಸದಿಶ) \
	apic_send_ipi (ಸದಿಶ, 0, APIC_IPI_ಗಮ್ಯ_ALL)
#ವ್ಯಾಖ್ಯಾನಿಸು apic_send_ipi_allbutself(ಸದಿಶ) \
	apic_send_ipi (ಸದಿಶ, 0, APIC_IPI_ಗಮ್ಯ_ALL_BUT_SELF);


#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>

#ವ್ಯಾಖ್ಯಾನಿಸು cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#ವ್ಯಾಖ್ಯಾನಿಸು lapic_read(what)	(*((ಅಸ್ಥಿರ u32_t *)((what))))
#ವ್ಯಾಖ್ಯಾನಿಸು lapic_write(what, ದತ್ತಾಂಶ)	ಮಾಡು {			\
	(*((ಅಸ್ಥಿರ u32_t *)((what)))) = ದತ್ತಾಂಶ;		\
} ಆಗ(0)

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* __APIC_H__ */
