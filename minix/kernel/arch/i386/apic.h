#جيڪڏهن_وصف_نه_ٿيل __APIC_H__
#وصف_ڪريو __APIC_H__

#وصف_ڪريو APIC_ENABLE		0x100
#وصف_ڪريو APIC_FOCUS_DISABLED	(1 << 9)
#وصف_ڪريو APIC_SIV		0xFF

#وصف_ڪريو APIC_TDCR_2	0x00
#وصف_ڪريو APIC_TDCR_4	0x01
#وصف_ڪريو APIC_TDCR_8	0x02
#وصف_ڪريو APIC_TDCR_16	0x03
#وصف_ڪريو APIC_TDCR_32	0x08
#وصف_ڪريو APIC_TDCR_64	0x09
#وصف_ڪريو APIC_TDCR_128	0x0a
#وصف_ڪريو APIC_TDCR_1	0x0b

#وصف_ڪريو APIC_LVTT_ويڪٽر_نقاب	0x000000FF
#وصف_ڪريو APIC_LVTT_DS_PENDING	(1 << 12)
#وصف_ڪريو APIC_LVTT_نقاب		(1 << 16)
#وصف_ڪريو APIC_LVTT_TM		(1 << 17)

#وصف_ڪريو APIC_LVT_IIPP_نقاب	0x00002000
#وصف_ڪريو APIC_LVT_IIPP_AH	0x00002000
#وصف_ڪريو APIC_LVT_IIPP_AL	0x00000000

#وصف_ڪريو IOAPIC_REGSEL		0x0
#وصف_ڪريو IOAPIC_RW		0x10

#وصف_ڪريو APIC_ICR_DM_نقاب		0x00000700
#وصف_ڪريو APIC_ICR_ويڪٽر			APIC_LVTT_ويڪٽر_نقاب
#وصف_ڪريو APIC_ICR_DM_FIXED		(0 << 8)
#وصف_ڪريو APIC_ICR_DM_سڀ_کان_هيٺ_ترجيح	(1 << 8)
#وصف_ڪريو APIC_ICR_DM_SMI			(2 << 8)
#وصف_ڪريو APIC_ICR_DM_RESERVED		(3 << 8)
#وصف_ڪريو APIC_ICR_DM_NMI			(4 << 8)
#وصف_ڪريو APIC_ICR_DM_INIT		(5 << 8)
#وصف_ڪريو APIC_ICR_DM_STARTUP		(6 << 8)
#وصف_ڪريو APIC_ICR_DM_EXTINT		(7 << 8)

#وصف_ڪريو APIC_ICR_DM_PHYSICAL		(0 << 11)
#وصف_ڪريو APIC_ICR_DM_LOGICAL		(1 << 11)

#وصف_ڪريو APIC_ICR_DELIVERY_PENDING	(1 << 12)

#وصف_ڪريو APIC_ICR_INT_POLARITY		(1 << 13)

#وصف_ڪريو APIC_ICR_LEVEL_ASSERT		(1 << 14)
#وصف_ڪريو APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#وصف_ڪريو APIC_ICR_TRIGGER		(1 << 15)

#وصف_ڪريو APIC_ICR_INT_نقاب		(1 << 16)

#وصف_ڪريو APIC_ICR_DEST_ميدان		(0 << 18)
#وصف_ڪريو APIC_ICR_DEST_SELF		(1 << 18)
#وصف_ڪريو APIC_ICR_DEST_ALL		(2 << 18)
#وصف_ڪريو APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#وصف_ڪريو LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#وصف_ڪريو IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#وصف_ڪريو LAPIC_ID	(lapic_addr + 0x020)
#وصف_ڪريو LAPIC_نسخو	(lapic_addr + 0x030)
#وصف_ڪريو LAPIC_TPR	(lapic_addr + 0x080)
#وصف_ڪريو LAPIC_EOI	(lapic_addr + 0x0b0)
#وصف_ڪريو LAPIC_LDR	(lapic_addr + 0x0d0)
#وصف_ڪريو LAPIC_DFR	(lapic_addr + 0x0e0)
#وصف_ڪريو LAPIC_SIVR	(lapic_addr + 0x0f0)
#وصف_ڪريو LAPIC_ISR	(lapic_addr + 0x100)
#وصف_ڪريو LAPIC_TMR	(lapic_addr + 0x180)
#وصف_ڪريو LAPIC_IRR	(lapic_addr + 0x200)
#وصف_ڪريو LAPIC_ESR	(lapic_addr + 0x280)
#وصف_ڪريو LAPIC_ICR1	(lapic_addr + 0x300)
#وصف_ڪريو LAPIC_ICR2	(lapic_addr + 0x310)
#وصف_ڪريو LAPIC_LVTTR	(lapic_addr + 0x320)
#وصف_ڪريو LAPIC_LVTTMR	(lapic_addr + 0x330)
#وصف_ڪريو LAPIC_LVTPCR	(lapic_addr + 0x340)
#وصف_ڪريو LAPIC_LINT0	(lapic_addr + 0x350)
#وصف_ڪريو LAPIC_LINT1	(lapic_addr + 0x360)
#وصف_ڪريو LAPIC_LVTER	(lapic_addr + 0x370)
#وصف_ڪريو LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#وصف_ڪريو LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#وصف_ڪريو LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#وصف_ڪريو IOAPIC_ID			0x0
#وصف_ڪريو IOAPIC_نسخو			0x1
#وصف_ڪريو IOAPIC_ARB			0x2
#وصف_ڪريو IOAPIC_REDIR_جدول		0x10

#وصف_ڪريو APIC_TIMER_INT_ويڪٽر		0xf0
#وصف_ڪريو APIC_SMP_SCHED_PROC_ويڪٽر	0xf1
#وصف_ڪريو APIC_SMP_CPU_HALT_ويڪٽر	0xf2
#وصف_ڪريو APIC_ERROR_INT_ويڪٽر		0xfe
#وصف_ڪريو APIC_SPURIOUS_INT_ويڪٽر	0xff

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

#ضمن "kernel/kernel.h"

EXTERN vir_بائيٽون lapic_addr;
EXTERN vir_بائيٽون lapic_eoi_addr;
EXTERN صحيح ioapic_enabled;
EXTERN صحيح bsp_lapic_id;

#وصف_ڪريو MAX_NR_IOAPICS		32
#وصف_ڪريو MAX_IOAPIC_IRQS		64

EXTERN صحيح ioapic_enabled;

بنية io_apic {
	بنانشان	id;
	vir_بائيٽون	addr; /* presently used address */
	phys_بائيٽون	paddr; /* where is it in phys space */
	vir_بائيٽون	vaddr; /* address after paging is on */
	بنانشان	pins;
	بنانشان	gsi_بنياد;
};

EXTERN بنية io_apic io_apic[MAX_NR_IOAPICS];
EXTERN بنانشان nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

صحيح lapic_enable(بنانشان cpu);
فراغ ioapic_unmask_irq(بنانشان irq);
فراغ ioapic_نقاب_irq(بنانشان irq);
فراغ ioapic_reset_pic(فراغ);

EXTERN صحيح ioapic_enabled;
EXTERN بنانشان nioapics;

فراغ lapic_microsec_sleep(بنانشان ڳڻپ);
فراغ ioapic_disable_irqs(u32_t irqs);
فراغ ioapic_enable_irqs(u32_t irqs);

صحيح lapic_enable(بنانشان cpu);
فراغ lapic_disable(فراغ);

فراغ ioapic_disable_all(فراغ);
صحيح ioapic_enable_all(فراغ);

صحيح detect_ioapics(فراغ);
فراغ apic_idt_init(صحيح reset);

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
صحيح apic_send_startup_ipi(بنانشان cpu, phys_بائيٽون trampoline);
صحيح apic_send_init_ipi(بنانشان cpu, phys_بائيٽون trampoline);
بنانشان صحيح apicid(فراغ);
فراغ ioapic_سيٽ_id(u32_t addr, بنانشان صحيح id);
#وإلا
صحيح apic_single_cpu_init(فراغ);
#جيڪڏهن_پورو

فراغ lapic_سيٽ_timer_periodic(ثابت بنانشان freq);
فراغ lapic_سيٽ_timer_one_shot(ثابت u32_t قدر);
فراغ lapic_stop_timer(فراغ);
فراغ lapic_restart_timer(فراغ);

فراغ ioapic_سيٽ_irq(بنانشان irq);
فراغ ioapic_unset_irq(بنانشان irq);

/* signal the end of interrupt handler to apic */
#وصف_ڪريو apic_eoi() افعل { *((متطاير u32_t *) lapic_eoi_addr) = 0; } طالما(0)

فراغ ioapic_eoi(صحيح irq);

فراغ dump_apic_irq_state(فراغ);

فراغ apic_send_ipi(بنانشان ويڪٽر, بنانشان cpu, صحيح type);

فراغ apic_ipi_sched_intr(فراغ);
فراغ apic_ipi_halt_intr(فراغ);

#وصف_ڪريو APIC_IPI_DEST			0
#وصف_ڪريو APIC_IPI_SELF			1
#وصف_ڪريو APIC_IPI_منزل_ALL			2
#وصف_ڪريو APIC_IPI_منزل_ALL_BUT_SELF	3

#وصف_ڪريو apic_send_ipi_single(ويڪٽر,cpu) \
	apic_send_ipi(ويڪٽر, cpu, APIC_IPI_DEST);
#وصف_ڪريو apic_send_ipi_self(ويڪٽر) \
	apic_send_ipi(ويڪٽر, 0, APIC_IPI_SELF)
#وصف_ڪريو apic_send_ipi_all(ويڪٽر) \
	apic_send_ipi (ويڪٽر, 0, APIC_IPI_منزل_ALL)
#وصف_ڪريو apic_send_ipi_allbutself(ويڪٽر) \
	apic_send_ipi (ويڪٽر, 0, APIC_IPI_منزل_ALL_BUT_SELF);


#ضمن <minix/cpufeature.h>

#وصف_ڪريو cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#وصف_ڪريو lapic_read(what)	(*((متطاير u32_t *)((what))))
#وصف_ڪريو lapic_write(what, معلومات)	افعل {			\
	(*((متطاير u32_t *)((what)))) = معلومات;		\
} طالما(0)

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو /* __APIC_H__ */
