#اگر_تعریف_نشده __APIC_H__
#تعریف_کن __APIC_H__

#تعریف_کن APIC_ENABLE		0x100
#تعریف_کن APIC_FOCUS_DISABLED	(1 << 9)
#تعریف_کن APIC_SIV		0xFF

#تعریف_کن APIC_TDCR_2	0x00
#تعریف_کن APIC_TDCR_4	0x01
#تعریف_کن APIC_TDCR_8	0x02
#تعریف_کن APIC_TDCR_16	0x03
#تعریف_کن APIC_TDCR_32	0x08
#تعریف_کن APIC_TDCR_64	0x09
#تعریف_کن APIC_TDCR_128	0x0a
#تعریف_کن APIC_TDCR_1	0x0b

#تعریف_کن APIC_LVTT_وکتور_ماسک	0x000000FF
#تعریف_کن APIC_LVTT_DS_PENDING	(1 << 12)
#تعریف_کن APIC_LVTT_ماسک		(1 << 16)
#تعریف_کن APIC_LVTT_TM		(1 << 17)

#تعریف_کن APIC_LVT_IIPP_ماسک	0x00002000
#تعریف_کن APIC_LVT_IIPP_AH	0x00002000
#تعریف_کن APIC_LVT_IIPP_AL	0x00000000

#تعریف_کن IOAPIC_REGSEL		0x0
#تعریف_کن IOAPIC_RW		0x10

#تعریف_کن APIC_ICR_DM_ماسک		0x00000700
#تعریف_کن APIC_ICR_وکتور			APIC_LVTT_وکتور_ماسک
#تعریف_کن APIC_ICR_DM_FIXED		(0 << 8)
#تعریف_کن APIC_ICR_DM_پایینترین_اولویت	(1 << 8)
#تعریف_کن APIC_ICR_DM_SMI			(2 << 8)
#تعریف_کن APIC_ICR_DM_RESERVED		(3 << 8)
#تعریف_کن APIC_ICR_DM_NMI			(4 << 8)
#تعریف_کن APIC_ICR_DM_INIT		(5 << 8)
#تعریف_کن APIC_ICR_DM_STARTUP		(6 << 8)
#تعریف_کن APIC_ICR_DM_EXTINT		(7 << 8)

#تعریف_کن APIC_ICR_DM_PHYSICAL		(0 << 11)
#تعریف_کن APIC_ICR_DM_LOGICAL		(1 << 11)

#تعریف_کن APIC_ICR_DELIVERY_PENDING	(1 << 12)

#تعریف_کن APIC_ICR_INT_POLARITY		(1 << 13)

#تعریف_کن APIC_ICR_LEVEL_ASSERT		(1 << 14)
#تعریف_کن APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#تعریف_کن APIC_ICR_TRIGGER		(1 << 15)

#تعریف_کن APIC_ICR_INT_ماسک		(1 << 16)

#تعریف_کن APIC_ICR_DEST_فیلد		(0 << 18)
#تعریف_کن APIC_ICR_DEST_SELF		(1 << 18)
#تعریف_کن APIC_ICR_DEST_ALL		(2 << 18)
#تعریف_کن APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#تعریف_کن LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#تعریف_کن IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#تعریف_کن LAPIC_ID	(lapic_addr + 0x020)
#تعریف_کن LAPIC_نسخه	(lapic_addr + 0x030)
#تعریف_کن LAPIC_TPR	(lapic_addr + 0x080)
#تعریف_کن LAPIC_EOI	(lapic_addr + 0x0b0)
#تعریف_کن LAPIC_LDR	(lapic_addr + 0x0d0)
#تعریف_کن LAPIC_DFR	(lapic_addr + 0x0e0)
#تعریف_کن LAPIC_SIVR	(lapic_addr + 0x0f0)
#تعریف_کن LAPIC_ISR	(lapic_addr + 0x100)
#تعریف_کن LAPIC_TMR	(lapic_addr + 0x180)
#تعریف_کن LAPIC_IRR	(lapic_addr + 0x200)
#تعریف_کن LAPIC_ESR	(lapic_addr + 0x280)
#تعریف_کن LAPIC_ICR1	(lapic_addr + 0x300)
#تعریف_کن LAPIC_ICR2	(lapic_addr + 0x310)
#تعریف_کن LAPIC_LVTTR	(lapic_addr + 0x320)
#تعریف_کن LAPIC_LVTTMR	(lapic_addr + 0x330)
#تعریف_کن LAPIC_LVTPCR	(lapic_addr + 0x340)
#تعریف_کن LAPIC_LINT0	(lapic_addr + 0x350)
#تعریف_کن LAPIC_LINT1	(lapic_addr + 0x360)
#تعریف_کن LAPIC_LVTER	(lapic_addr + 0x370)
#تعریف_کن LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#تعریف_کن LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#تعریف_کن LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#تعریف_کن IOAPIC_ID			0x0
#تعریف_کن IOAPIC_نسخه			0x1
#تعریف_کن IOAPIC_ARB			0x2
#تعریف_کن IOAPIC_REDIR_جدول		0x10

#تعریف_کن APIC_TIMER_INT_وکتور		0xf0
#تعریف_کن APIC_SMP_SCHED_PROC_وکتور	0xf1
#تعریف_کن APIC_SMP_CPU_HALT_وکتور	0xf2
#تعریف_کن APIC_ERROR_INT_وکتور		0xfe
#تعریف_کن APIC_SPURIOUS_INT_وکتور	0xff

#اگر_تعریف_نشده __ASSEMBLY__

#ضمن "kernel/kernel.h"

EXTERN vir_بایتها lapic_addr;
EXTERN vir_بایتها lapic_eoi_addr;
EXTERN صحيح ioapic_enabled;
EXTERN صحيح bsp_lapic_id;

#تعریف_کن MAX_NR_IOAPICS		32
#تعریف_کن MAX_IOAPIC_IRQS		64

EXTERN صحيح ioapic_enabled;

بنية io_apic {
	بیعلامت	id;
	vir_بایتها	addr; /* presently used address */
	phys_بایتها	paddr; /* where is it in phys space */
	vir_بایتها	vaddr; /* address after paging is on */
	بیعلامت	pins;
	بیعلامت	gsi_پایه;
};

EXTERN بنية io_apic io_apic[MAX_NR_IOAPICS];
EXTERN بیعلامت nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

صحيح lapic_enable(بیعلامت cpu);
فراغ ioapic_unmask_irq(بیعلامت irq);
فراغ ioapic_ماسک_irq(بیعلامت irq);
فراغ ioapic_reset_pic(فراغ);

EXTERN صحيح ioapic_enabled;
EXTERN بیعلامت nioapics;

فراغ lapic_microsec_sleep(بیعلامت شمار);
فراغ ioapic_disable_irqs(u32_t irqs);
فراغ ioapic_enable_irqs(u32_t irqs);

صحيح lapic_enable(بیعلامت cpu);
فراغ lapic_disable(فراغ);

فراغ ioapic_disable_all(فراغ);
صحيح ioapic_enable_all(فراغ);

صحيح detect_ioapics(فراغ);
فراغ apic_idt_init(صحيح reset);

#اگر_تعریف_شده CONFIG_SMP
صحيح apic_send_startup_ipi(بیعلامت cpu, phys_بایتها trampoline);
صحيح apic_send_init_ipi(بیعلامت cpu, phys_بایتها trampoline);
بیعلامت صحيح apicid(فراغ);
فراغ ioapic_مجموعه_id(u32_t addr, بیعلامت صحيح id);
#وإلا
صحيح apic_single_cpu_init(فراغ);
#پایان_اگر

فراغ lapic_مجموعه_timer_periodic(ثابت بیعلامت freq);
فراغ lapic_مجموعه_timer_one_shot(ثابت u32_t مقدار);
فراغ lapic_stop_timer(فراغ);
فراغ lapic_restart_timer(فراغ);

فراغ ioapic_مجموعه_irq(بیعلامت irq);
فراغ ioapic_unset_irq(بیعلامت irq);

/* signal the end of interrupt handler to apic */
#تعریف_کن apic_eoi() افعل { *((متطاير u32_t *) lapic_eoi_addr) = 0; } طالما(0)

فراغ ioapic_eoi(صحيح irq);

فراغ dump_apic_irq_state(فراغ);

فراغ apic_send_ipi(بیعلامت وکتور, بیعلامت cpu, صحيح type);

فراغ apic_ipi_sched_intr(فراغ);
فراغ apic_ipi_halt_intr(فراغ);

#تعریف_کن APIC_IPI_DEST			0
#تعریف_کن APIC_IPI_SELF			1
#تعریف_کن APIC_IPI_مقصد_ALL			2
#تعریف_کن APIC_IPI_مقصد_ALL_BUT_SELF	3

#تعریف_کن apic_send_ipi_single(وکتور,cpu) \
	apic_send_ipi(وکتور, cpu, APIC_IPI_DEST);
#تعریف_کن apic_send_ipi_self(وکتور) \
	apic_send_ipi(وکتور, 0, APIC_IPI_SELF)
#تعریف_کن apic_send_ipi_all(وکتور) \
	apic_send_ipi (وکتور, 0, APIC_IPI_مقصد_ALL)
#تعریف_کن apic_send_ipi_allbutself(وکتور) \
	apic_send_ipi (وکتور, 0, APIC_IPI_مقصد_ALL_BUT_SELF);


#ضمن <minix/cpufeature.h>

#تعریف_کن cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#تعریف_کن lapic_read(what)	(*((متطاير u32_t *)((what))))
#تعریف_کن lapic_write(what, داده)	افعل {			\
	(*((متطاير u32_t *)((what)))) = داده;		\
} طالما(0)

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* __APIC_H__ */
