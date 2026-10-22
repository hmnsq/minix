#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __APIC_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __APIC_H__

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ENABLE		0x100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_FOCUS_DISABLED	(1 << 9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_SIV		0xFF

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_2	0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_4	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_8	0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_16	0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_32	0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_64	0x09
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_128	0x0a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TDCR_1	0x0b

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_ਸਦਿਸ਼_ਨਕਾਬ	0x000000FF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_DS_PENDING	(1 << 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_ਨਕਾਬ		(1 << 16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_TM		(1 << 17)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_ਨਕਾਬ	0x00002000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_AH	0x00002000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_AL	0x00000000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REGSEL		0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_RW		0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_ਨਕਾਬ		0x00000700
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_ਸਦਿਸ਼			APIC_LVTT_ਸਦਿਸ਼_ਨਕਾਬ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_FIXED		(0 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਤਰਜੀਹ	(1 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_SMI			(2 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_RESERVED		(3 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_NMI			(4 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_INIT		(5 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_STARTUP		(6 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_EXTINT		(7 << 8)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_PHYSICAL		(0 << 11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DM_LOGICAL		(1 << 11)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DELIVERY_PENDING	(1 << 12)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_INT_POLARITY		(1 << 13)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_LEVEL_ASSERT		(1 << 14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_TRIGGER		(1 << 15)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_INT_ਨਕਾਬ		(1 << 16)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ਖੇਤਰ		(0 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_SELF		(1 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ALL		(2 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ID	(lapic_addr + 0x020)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ਸੰਸਕਰਣ	(lapic_addr + 0x030)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_TPR	(lapic_addr + 0x080)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_EOI	(lapic_addr + 0x0b0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LDR	(lapic_addr + 0x0d0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_DFR	(lapic_addr + 0x0e0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_SIVR	(lapic_addr + 0x0f0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ISR	(lapic_addr + 0x100)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_TMR	(lapic_addr + 0x180)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_IRR	(lapic_addr + 0x200)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ESR	(lapic_addr + 0x280)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ICR1	(lapic_addr + 0x300)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ICR2	(lapic_addr + 0x310)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LVTTR	(lapic_addr + 0x320)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LVTTMR	(lapic_addr + 0x330)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LVTPCR	(lapic_addr + 0x340)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LINT0	(lapic_addr + 0x350)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LINT1	(lapic_addr + 0x360)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_LVTER	(lapic_addr + 0x370)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_ID			0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_ਸੰਸਕਰਣ			0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_ARB			0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REDIR_ਸਾਰਣੀ		0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TIMER_INT_ਸਦਿਸ਼		0xf0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_SMP_SCHED_PROC_ਸਦਿਸ਼	0xf1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_SMP_CPU_HALT_ਸਦਿਸ਼	0xf2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ERROR_INT_ਸਦਿਸ਼		0xfe
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_SPURIOUS_INT_ਸਦਿਸ਼	0xff

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

EXTERN vir_ਬਾਈਟਾਂ lapic_addr;
EXTERN vir_ਬਾਈਟਾਂ lapic_eoi_addr;
EXTERN ਪੂਰਨ ioapic_enabled;
EXTERN ਪੂਰਨ bsp_lapic_id;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NR_IOAPICS		32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_IOAPIC_IRQS		64

EXTERN ਪੂਰਨ ioapic_enabled;

ਬਣਤਰ io_apic {
	ਬਿਨਾਨਿਸ਼ਾਨੀ	id;
	vir_ਬਾਈਟਾਂ	addr; /* presently used address */
	phys_ਬਾਈਟਾਂ	paddr; /* where is it in phys space */
	vir_ਬਾਈਟਾਂ	vaddr; /* address after paging is on */
	ਬਿਨਾਨਿਸ਼ਾਨੀ	pins;
	ਬਿਨਾਨਿਸ਼ਾਨੀ	gsi_ਅਧਾਰ;
};

EXTERN ਬਣਤਰ io_apic io_apic[MAX_NR_IOAPICS];
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

ਪੂਰਨ lapic_enable(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
ਖਾਲੀ ioapic_unmask_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq);
ਖਾਲੀ ioapic_ਨਕਾਬ_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq);
ਖਾਲੀ ioapic_reset_pic(ਖਾਲੀ);

EXTERN ਪੂਰਨ ioapic_enabled;
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ nioapics;

ਖਾਲੀ lapic_microsec_sleep(ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2);
ਖਾਲੀ ioapic_disable_irqs(u32_t irqs);
ਖਾਲੀ ioapic_enable_irqs(u32_t irqs);

ਪੂਰਨ lapic_enable(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
ਖਾਲੀ lapic_disable(ਖਾਲੀ);

ਖਾਲੀ ioapic_disable_all(ਖਾਲੀ);
ਪੂਰਨ ioapic_enable_all(ਖਾਲੀ);

ਪੂਰਨ detect_ioapics(ਖਾਲੀ);
ਖਾਲੀ apic_idt_init(ਪੂਰਨ reset);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਪੂਰਨ apic_send_startup_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, phys_ਬਾਈਟਾਂ trampoline);
ਪੂਰਨ apic_send_init_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, phys_ਬਾਈਟਾਂ trampoline);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ apicid(ਖਾਲੀ);
ਖਾਲੀ ioapic_ਸੈੱਟ_id(u32_t addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ id);
#ਨਹੀਂਤਾਂ
ਪੂਰਨ apic_single_cpu_init(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਖਾਲੀ lapic_ਸੈੱਟ_timer_periodic(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq);
ਖਾਲੀ lapic_ਸੈੱਟ_timer_one_shot(ਅਚਲ u32_t ਮੁੱਲ);
ਖਾਲੀ lapic_stop_timer(ਖਾਲੀ);
ਖਾਲੀ lapic_restart_timer(ਖਾਲੀ);

ਖਾਲੀ ioapic_ਸੈੱਟ_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq);
ਖਾਲੀ ioapic_unset_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq);

/* signal the end of interrupt handler to apic */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_eoi() ਕਰੋ { *((ਅਸਥਿਰ u32_t *) lapic_eoi_addr) = 0; } ਜਦੋਂ(0)

ਖਾਲੀ ioapic_eoi(ਪੂਰਨ irq);

ਖਾਲੀ dump_apic_irq_state(ਖਾਲੀ);

ਖਾਲੀ apic_send_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਪੂਰਨ type);

ਖਾਲੀ apic_ipi_sched_intr(ਖਾਲੀ);
ਖਾਲੀ apic_ipi_halt_intr(ਖਾਲੀ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_IPI_DEST			0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_IPI_SELF			1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_IPI_ਮੰਜ਼ਿਲ_ALL			2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_IPI_ਮੰਜ਼ਿਲ_ALL_BUT_SELF	3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_send_ipi_single(ਸਦਿਸ਼,cpu) \
	apic_send_ipi(ਸਦਿਸ਼, cpu, APIC_IPI_DEST);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_send_ipi_self(ਸਦਿਸ਼) \
	apic_send_ipi(ਸਦਿਸ਼, 0, APIC_IPI_SELF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_send_ipi_all(ਸਦਿਸ਼) \
	apic_send_ipi (ਸਦਿਸ਼, 0, APIC_IPI_ਮੰਜ਼ਿਲ_ALL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_send_ipi_allbutself(ਸਦਿਸ਼) \
	apic_send_ipi (ਸਦਿਸ਼, 0, APIC_IPI_ਮੰਜ਼ਿਲ_ALL_BUT_SELF);


#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_read(what)	(*((ਅਸਥਿਰ u32_t *)((what))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_write(what, ਅੰਕੜੇ)	ਕਰੋ {			\
	(*((ਅਸਥਿਰ u32_t *)((what)))) = ਅੰਕੜੇ;		\
} ਜਦੋਂ(0)

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* __APIC_H__ */
