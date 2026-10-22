#اگر_غیر_تعریف_شدہ __APIC_H__
#تعریف_کرو __APIC_H__

#تعریف_کرو APIC_ENABLE		0x100
#تعریف_کرو APIC_FOCUS_DISABLED	(1 << 9)
#تعریف_کرو APIC_SIV		0xFF

#تعریف_کرو APIC_TDCR_2	0x00
#تعریف_کرو APIC_TDCR_4	0x01
#تعریف_کرو APIC_TDCR_8	0x02
#تعریف_کرو APIC_TDCR_16	0x03
#تعریف_کرو APIC_TDCR_32	0x08
#تعریف_کرو APIC_TDCR_64	0x09
#تعریف_کرو APIC_TDCR_128	0x0a
#تعریف_کرو APIC_TDCR_1	0x0b

#تعریف_کرو APIC_LVTT_سمتیہ_نقاب	0x000000FF
#تعریف_کرو APIC_LVTT_DS_PENDING	(1 << 12)
#تعریف_کرو APIC_LVTT_نقاب		(1 << 16)
#تعریف_کرو APIC_LVTT_TM		(1 << 17)

#تعریف_کرو APIC_LVT_IIPP_نقاب	0x00002000
#تعریف_کرو APIC_LVT_IIPP_AH	0x00002000
#تعریف_کرو APIC_LVT_IIPP_AL	0x00000000

#تعریف_کرو IOAPIC_REGSEL		0x0
#تعریف_کرو IOAPIC_RW		0x10

#تعریف_کرو APIC_ICR_DM_نقاب		0x00000700
#تعریف_کرو APIC_ICR_سمتیہ			APIC_LVTT_سمتیہ_نقاب
#تعریف_کرو APIC_ICR_DM_FIXED		(0 << 8)
#تعریف_کرو APIC_ICR_DM_سب_سے_نیچا_ترجیح	(1 << 8)
#تعریف_کرو APIC_ICR_DM_SMI			(2 << 8)
#تعریف_کرو APIC_ICR_DM_RESERVED		(3 << 8)
#تعریف_کرو APIC_ICR_DM_NMI			(4 << 8)
#تعریف_کرو APIC_ICR_DM_INIT		(5 << 8)
#تعریف_کرو APIC_ICR_DM_STARTUP		(6 << 8)
#تعریف_کرو APIC_ICR_DM_EXTINT		(7 << 8)

#تعریف_کرو APIC_ICR_DM_PHYSICAL		(0 << 11)
#تعریف_کرو APIC_ICR_DM_LOGICAL		(1 << 11)

#تعریف_کرو APIC_ICR_DELIVERY_PENDING	(1 << 12)

#تعریف_کرو APIC_ICR_INT_POLARITY		(1 << 13)

#تعریف_کرو APIC_ICR_LEVEL_ASSERT		(1 << 14)
#تعریف_کرو APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#تعریف_کرو APIC_ICR_TRIGGER		(1 << 15)

#تعریف_کرو APIC_ICR_INT_نقاب		(1 << 16)

#تعریف_کرو APIC_ICR_DEST_میدان		(0 << 18)
#تعریف_کرو APIC_ICR_DEST_SELF		(1 << 18)
#تعریف_کرو APIC_ICR_DEST_ALL		(2 << 18)
#تعریف_کرو APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#تعریف_کرو LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#تعریف_کرو IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#تعریف_کرو LAPIC_ID	(lapic_addr + 0x020)
#تعریف_کرو LAPIC_نسخہ	(lapic_addr + 0x030)
#تعریف_کرو LAPIC_TPR	(lapic_addr + 0x080)
#تعریف_کرو LAPIC_EOI	(lapic_addr + 0x0b0)
#تعریف_کرو LAPIC_LDR	(lapic_addr + 0x0d0)
#تعریف_کرو LAPIC_DFR	(lapic_addr + 0x0e0)
#تعریف_کرو LAPIC_SIVR	(lapic_addr + 0x0f0)
#تعریف_کرو LAPIC_ISR	(lapic_addr + 0x100)
#تعریف_کرو LAPIC_TMR	(lapic_addr + 0x180)
#تعریف_کرو LAPIC_IRR	(lapic_addr + 0x200)
#تعریف_کرو LAPIC_ESR	(lapic_addr + 0x280)
#تعریف_کرو LAPIC_ICR1	(lapic_addr + 0x300)
#تعریف_کرو LAPIC_ICR2	(lapic_addr + 0x310)
#تعریف_کرو LAPIC_LVTTR	(lapic_addr + 0x320)
#تعریف_کرو LAPIC_LVTTMR	(lapic_addr + 0x330)
#تعریف_کرو LAPIC_LVTPCR	(lapic_addr + 0x340)
#تعریف_کرو LAPIC_LINT0	(lapic_addr + 0x350)
#تعریف_کرو LAPIC_LINT1	(lapic_addr + 0x360)
#تعریف_کرو LAPIC_LVTER	(lapic_addr + 0x370)
#تعریف_کرو LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#تعریف_کرو LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#تعریف_کرو LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#تعریف_کرو IOAPIC_ID			0x0
#تعریف_کرو IOAPIC_نسخہ			0x1
#تعریف_کرو IOAPIC_ARB			0x2
#تعریف_کرو IOAPIC_REDIR_جدول		0x10

#تعریف_کرو APIC_TIMER_INT_سمتیہ		0xf0
#تعریف_کرو APIC_SMP_SCHED_PROC_سمتیہ	0xf1
#تعریف_کرو APIC_SMP_CPU_HALT_سمتیہ	0xf2
#تعریف_کرو APIC_ERROR_INT_سمتیہ		0xfe
#تعریف_کرو APIC_SPURIOUS_INT_سمتیہ	0xff

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

#شامل "kernel/kernel.h"

EXTERN vir_بائٹیں lapic_addr;
EXTERN vir_بائٹیں lapic_eoi_addr;
EXTERN صحیح ioapic_enabled;
EXTERN صحیح bsp_lapic_id;

#تعریف_کرو MAX_NR_IOAPICS		32
#تعریف_کرو MAX_IOAPIC_IRQS		64

EXTERN صحیح ioapic_enabled;

ساخت io_apic {
	غیرعلامتی	id;
	vir_بائٹیں	addr; /* presently used address */
	phys_بائٹیں	paddr; /* where is it in phys space */
	vir_بائٹیں	vaddr; /* address after paging is on */
	غیرعلامتی	pins;
	غیرعلامتی	gsi_بنیاد;
};

EXTERN ساخت io_apic io_apic[MAX_NR_IOAPICS];
EXTERN غیرعلامتی nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

صحیح lapic_enable(غیرعلامتی cpu);
خالی ioapic_unmask_irq(غیرعلامتی irq);
خالی ioapic_نقاب_irq(غیرعلامتی irq);
خالی ioapic_reset_pic(خالی);

EXTERN صحیح ioapic_enabled;
EXTERN غیرعلامتی nioapics;

خالی lapic_microsec_sleep(غیرعلامتی گنتی);
خالی ioapic_disable_irqs(u32_t irqs);
خالی ioapic_enable_irqs(u32_t irqs);

صحیح lapic_enable(غیرعلامتی cpu);
خالی lapic_disable(خالی);

خالی ioapic_disable_all(خالی);
صحیح ioapic_enable_all(خالی);

صحیح detect_ioapics(خالی);
خالی apic_idt_init(صحیح reset);

#اگر_تعریف_شدہ CONFIG_SMP
صحیح apic_send_startup_ipi(غیرعلامتی cpu, phys_بائٹیں trampoline);
صحیح apic_send_init_ipi(غیرعلامتی cpu, phys_بائٹیں trampoline);
غیرعلامتی صحیح apicid(خالی);
خالی ioapic_سیٹ_id(u32_t addr, غیرعلامتی صحیح id);
#ورنہ
صحیح apic_single_cpu_init(خالی);
#اگر_ختم

خالی lapic_سیٹ_timer_periodic(ثابت غیرعلامتی freq);
خالی lapic_سیٹ_timer_one_shot(ثابت u32_t قدر);
خالی lapic_stop_timer(خالی);
خالی lapic_restart_timer(خالی);

خالی ioapic_سیٹ_irq(غیرعلامتی irq);
خالی ioapic_unset_irq(غیرعلامتی irq);

/* signal the end of interrupt handler to apic */
#تعریف_کرو apic_eoi() کرو { *((متلون u32_t *) lapic_eoi_addr) = 0; } جب(0)

خالی ioapic_eoi(صحیح irq);

خالی dump_apic_irq_state(خالی);

خالی apic_send_ipi(غیرعلامتی سمتیہ, غیرعلامتی cpu, صحیح type);

خالی apic_ipi_sched_intr(خالی);
خالی apic_ipi_halt_intr(خالی);

#تعریف_کرو APIC_IPI_DEST			0
#تعریف_کرو APIC_IPI_SELF			1
#تعریف_کرو APIC_IPI_مقصد_ALL			2
#تعریف_کرو APIC_IPI_مقصد_ALL_BUT_SELF	3

#تعریف_کرو apic_send_ipi_single(سمتیہ,cpu) \
	apic_send_ipi(سمتیہ, cpu, APIC_IPI_DEST);
#تعریف_کرو apic_send_ipi_self(سمتیہ) \
	apic_send_ipi(سمتیہ, 0, APIC_IPI_SELF)
#تعریف_کرو apic_send_ipi_all(سمتیہ) \
	apic_send_ipi (سمتیہ, 0, APIC_IPI_مقصد_ALL)
#تعریف_کرو apic_send_ipi_allbutself(سمتیہ) \
	apic_send_ipi (سمتیہ, 0, APIC_IPI_مقصد_ALL_BUT_SELF);


#شامل <minix/cpufeature.h>

#تعریف_کرو cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#تعریف_کرو lapic_read(what)	(*((متلون u32_t *)((what))))
#تعریف_کرو lapic_write(what, ڈیٹا)	کرو {			\
	(*((متلون u32_t *)((what)))) = ڈیٹا;		\
} جب(0)

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* __APIC_H__ */
