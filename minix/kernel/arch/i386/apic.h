#که_نه_تعریف_شوی __APIC_H__
#تعریف_کړه __APIC_H__

#تعریف_کړه APIC_ENABLE		0x100
#تعریف_کړه APIC_FOCUS_DISABLED	(1 << 9)
#تعریف_کړه APIC_SIV		0xFF

#تعریف_کړه APIC_TDCR_2	0x00
#تعریف_کړه APIC_TDCR_4	0x01
#تعریف_کړه APIC_TDCR_8	0x02
#تعریف_کړه APIC_TDCR_16	0x03
#تعریف_کړه APIC_TDCR_32	0x08
#تعریف_کړه APIC_TDCR_64	0x09
#تعریف_کړه APIC_TDCR_128	0x0a
#تعریف_کړه APIC_TDCR_1	0x0b

#تعریف_کړه APIC_LVTT_ویکټور_ماسک	0x000000FF
#تعریف_کړه APIC_LVTT_DS_PENDING	(1 << 12)
#تعریف_کړه APIC_LVTT_ماسک		(1 << 16)
#تعریف_کړه APIC_LVTT_TM		(1 << 17)

#تعریف_کړه APIC_LVT_IIPP_ماسک	0x00002000
#تعریف_کړه APIC_LVT_IIPP_AH	0x00002000
#تعریف_کړه APIC_LVT_IIPP_AL	0x00000000

#تعریف_کړه IOAPIC_REGSEL		0x0
#تعریف_کړه IOAPIC_RW		0x10

#تعریف_کړه APIC_ICR_DM_ماسک		0x00000700
#تعریف_کړه APIC_ICR_ویکټور			APIC_LVTT_ویکټور_ماسک
#تعریف_کړه APIC_ICR_DM_FIXED		(0 << 8)
#تعریف_کړه APIC_ICR_DM_ټیټ_ترین_لومړیتوب	(1 << 8)
#تعریف_کړه APIC_ICR_DM_SMI			(2 << 8)
#تعریف_کړه APIC_ICR_DM_RESERVED		(3 << 8)
#تعریف_کړه APIC_ICR_DM_NMI			(4 << 8)
#تعریف_کړه APIC_ICR_DM_INIT		(5 << 8)
#تعریف_کړه APIC_ICR_DM_STARTUP		(6 << 8)
#تعریف_کړه APIC_ICR_DM_EXTINT		(7 << 8)

#تعریف_کړه APIC_ICR_DM_PHYSICAL		(0 << 11)
#تعریف_کړه APIC_ICR_DM_LOGICAL		(1 << 11)

#تعریف_کړه APIC_ICR_DELIVERY_PENDING	(1 << 12)

#تعریف_کړه APIC_ICR_INT_POLARITY		(1 << 13)

#تعریف_کړه APIC_ICR_LEVEL_ASSERT		(1 << 14)
#تعریف_کړه APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#تعریف_کړه APIC_ICR_TRIGGER		(1 << 15)

#تعریف_کړه APIC_ICR_INT_ماسک		(1 << 16)

#تعریف_کړه APIC_ICR_DEST_ډګر		(0 << 18)
#تعریف_کړه APIC_ICR_DEST_SELF		(1 << 18)
#تعریف_کړه APIC_ICR_DEST_ALL		(2 << 18)
#تعریف_کړه APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#تعریف_کړه LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#تعریف_کړه IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#تعریف_کړه LAPIC_ID	(lapic_addr + 0x020)
#تعریف_کړه LAPIC_نسخه	(lapic_addr + 0x030)
#تعریف_کړه LAPIC_TPR	(lapic_addr + 0x080)
#تعریف_کړه LAPIC_EOI	(lapic_addr + 0x0b0)
#تعریف_کړه LAPIC_LDR	(lapic_addr + 0x0d0)
#تعریف_کړه LAPIC_DFR	(lapic_addr + 0x0e0)
#تعریف_کړه LAPIC_SIVR	(lapic_addr + 0x0f0)
#تعریف_کړه LAPIC_ISR	(lapic_addr + 0x100)
#تعریف_کړه LAPIC_TMR	(lapic_addr + 0x180)
#تعریف_کړه LAPIC_IRR	(lapic_addr + 0x200)
#تعریف_کړه LAPIC_ESR	(lapic_addr + 0x280)
#تعریف_کړه LAPIC_ICR1	(lapic_addr + 0x300)
#تعریف_کړه LAPIC_ICR2	(lapic_addr + 0x310)
#تعریف_کړه LAPIC_LVTTR	(lapic_addr + 0x320)
#تعریف_کړه LAPIC_LVTTMR	(lapic_addr + 0x330)
#تعریف_کړه LAPIC_LVTPCR	(lapic_addr + 0x340)
#تعریف_کړه LAPIC_LINT0	(lapic_addr + 0x350)
#تعریف_کړه LAPIC_LINT1	(lapic_addr + 0x360)
#تعریف_کړه LAPIC_LVTER	(lapic_addr + 0x370)
#تعریف_کړه LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#تعریف_کړه LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#تعریف_کړه LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#تعریف_کړه IOAPIC_ID			0x0
#تعریف_کړه IOAPIC_نسخه			0x1
#تعریف_کړه IOAPIC_ARB			0x2
#تعریف_کړه IOAPIC_REDIR_جدول		0x10

#تعریف_کړه APIC_TIMER_INT_ویکټور		0xf0
#تعریف_کړه APIC_SMP_SCHED_PROC_ویکټور	0xf1
#تعریف_کړه APIC_SMP_CPU_HALT_ویکټور	0xf2
#تعریف_کړه APIC_ERROR_INT_ویکټور		0xfe
#تعریف_کړه APIC_SPURIOUS_INT_ویکټور	0xff

#که_نه_تعریف_شوی __ASSEMBLY__

#ضمن "kernel/kernel.h"

EXTERN vir_بایټونه lapic_addr;
EXTERN vir_بایټونه lapic_eoi_addr;
EXTERN صحيح ioapic_enabled;
EXTERN صحيح bsp_lapic_id;

#تعریف_کړه MAX_NR_IOAPICS		32
#تعریف_کړه MAX_IOAPIC_IRQS		64

EXTERN صحيح ioapic_enabled;

بنية io_apic {
	بینښه	id;
	vir_بایټونه	addr; /* presently used address */
	phys_بایټونه	paddr; /* where is it in phys space */
	vir_بایټونه	vaddr; /* address after paging is on */
	بینښه	pins;
	بینښه	gsi_بنسټ;
};

EXTERN بنية io_apic io_apic[MAX_NR_IOAPICS];
EXTERN بینښه nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

صحيح lapic_enable(بینښه cpu);
فراغ ioapic_unmask_irq(بینښه irq);
فراغ ioapic_ماسک_irq(بینښه irq);
فراغ ioapic_reset_pic(فراغ);

EXTERN صحيح ioapic_enabled;
EXTERN بینښه nioapics;

فراغ lapic_microsec_sleep(بینښه شمېر);
فراغ ioapic_disable_irqs(u32_t irqs);
فراغ ioapic_enable_irqs(u32_t irqs);

صحيح lapic_enable(بینښه cpu);
فراغ lapic_disable(فراغ);

فراغ ioapic_disable_all(فراغ);
صحيح ioapic_enable_all(فراغ);

صحيح detect_ioapics(فراغ);
فراغ apic_idt_init(صحيح reset);

#که_تعریف_شوی CONFIG_SMP
صحيح apic_send_startup_ipi(بینښه cpu, phys_بایټونه trampoline);
صحيح apic_send_init_ipi(بینښه cpu, phys_بایټونه trampoline);
بینښه صحيح apicid(فراغ);
فراغ ioapic_سېټ_id(u32_t addr, بینښه صحيح id);
#وإلا
صحيح apic_single_cpu_init(فراغ);
#د_که_پای

فراغ lapic_سېټ_timer_periodic(ثابت بینښه freq);
فراغ lapic_سېټ_timer_one_shot(ثابت u32_t ارزښت);
فراغ lapic_stop_timer(فراغ);
فراغ lapic_restart_timer(فراغ);

فراغ ioapic_سېټ_irq(بینښه irq);
فراغ ioapic_unset_irq(بینښه irq);

/* signal the end of interrupt handler to apic */
#تعریف_کړه apic_eoi() افعل { *((متطاير u32_t *) lapic_eoi_addr) = 0; } طالما(0)

فراغ ioapic_eoi(صحيح irq);

فراغ dump_apic_irq_state(فراغ);

فراغ apic_send_ipi(بینښه ویکټور, بینښه cpu, صحيح type);

فراغ apic_ipi_sched_intr(فراغ);
فراغ apic_ipi_halt_intr(فراغ);

#تعریف_کړه APIC_IPI_DEST			0
#تعریف_کړه APIC_IPI_SELF			1
#تعریف_کړه APIC_IPI_منزل_ALL			2
#تعریف_کړه APIC_IPI_منزل_ALL_BUT_SELF	3

#تعریف_کړه apic_send_ipi_single(ویکټور,cpu) \
	apic_send_ipi(ویکټور, cpu, APIC_IPI_DEST);
#تعریف_کړه apic_send_ipi_self(ویکټور) \
	apic_send_ipi(ویکټور, 0, APIC_IPI_SELF)
#تعریف_کړه apic_send_ipi_all(ویکټور) \
	apic_send_ipi (ویکټور, 0, APIC_IPI_منزل_ALL)
#تعریف_کړه apic_send_ipi_allbutself(ویکټور) \
	apic_send_ipi (ویکټور, 0, APIC_IPI_منزل_ALL_BUT_SELF);


#ضمن <minix/cpufeature.h>

#تعریف_کړه cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#تعریف_کړه lapic_read(what)	(*((متطاير u32_t *)((what))))
#تعریف_کړه lapic_write(what, معلومات)	افعل {			\
	(*((متطاير u32_t *)((what)))) = معلومات;		\
} طالما(0)

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای /* __APIC_H__ */
