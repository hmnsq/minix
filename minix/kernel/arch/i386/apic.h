#यदि_अपरिभाषितम् __APIC_H__
#परिभाषय __APIC_H__

#परिभाषय APIC_ENABLE		0x100
#परिभाषय APIC_FOCUS_DISABLED	(1 << 9)
#परिभाषय APIC_SIV		0xFF

#परिभाषय APIC_TDCR_2	0x00
#परिभाषय APIC_TDCR_4	0x01
#परिभाषय APIC_TDCR_8	0x02
#परिभाषय APIC_TDCR_16	0x03
#परिभाषय APIC_TDCR_32	0x08
#परिभाषय APIC_TDCR_64	0x09
#परिभाषय APIC_TDCR_128	0x0a
#परिभाषय APIC_TDCR_1	0x0b

#परिभाषय APIC_LVTT_सदिशम्_आवरणम्	0x000000FF
#परिभाषय APIC_LVTT_DS_PENDING	(1 << 12)
#परिभाषय APIC_LVTT_आवरणम्		(1 << 16)
#परिभाषय APIC_LVTT_TM		(1 << 17)

#परिभाषय APIC_LVT_IIPP_आवरणम्	0x00002000
#परिभाषय APIC_LVT_IIPP_AH	0x00002000
#परिभाषय APIC_LVT_IIPP_AL	0x00000000

#परिभाषय IOAPIC_REGSEL		0x0
#परिभाषय IOAPIC_RW		0x10

#परिभाषय APIC_ICR_DM_आवरणम्		0x00000700
#परिभाषय APIC_ICR_सदिशम्			APIC_LVTT_सदिशम्_आवरणम्
#परिभाषय APIC_ICR_DM_FIXED		(0 << 8)
#परिभाषय APIC_ICR_DM_निम्नतमम्_प्राथमिकता	(1 << 8)
#परिभाषय APIC_ICR_DM_SMI			(2 << 8)
#परिभाषय APIC_ICR_DM_RESERVED		(3 << 8)
#परिभाषय APIC_ICR_DM_NMI			(4 << 8)
#परिभाषय APIC_ICR_DM_INIT		(5 << 8)
#परिभाषय APIC_ICR_DM_STARTUP		(6 << 8)
#परिभाषय APIC_ICR_DM_EXTINT		(7 << 8)

#परिभाषय APIC_ICR_DM_PHYSICAL		(0 << 11)
#परिभाषय APIC_ICR_DM_LOGICAL		(1 << 11)

#परिभाषय APIC_ICR_DELIVERY_PENDING	(1 << 12)

#परिभाषय APIC_ICR_INT_POLARITY		(1 << 13)

#परिभाषय APIC_ICR_LEVEL_ASSERT		(1 << 14)
#परिभाषय APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#परिभाषय APIC_ICR_TRIGGER		(1 << 15)

#परिभाषय APIC_ICR_INT_आवरणम्		(1 << 16)

#परिभाषय APIC_ICR_DEST_क्षेत्रम्		(0 << 18)
#परिभाषय APIC_ICR_DEST_SELF		(1 << 18)
#परिभाषय APIC_ICR_DEST_ALL		(2 << 18)
#परिभाषय APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#परिभाषय LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#परिभाषय IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#परिभाषय LAPIC_ID	(lapic_addr + 0x020)
#परिभाषय LAPIC_संस्करणम्	(lapic_addr + 0x030)
#परिभाषय LAPIC_TPR	(lapic_addr + 0x080)
#परिभाषय LAPIC_EOI	(lapic_addr + 0x0b0)
#परिभाषय LAPIC_LDR	(lapic_addr + 0x0d0)
#परिभाषय LAPIC_DFR	(lapic_addr + 0x0e0)
#परिभाषय LAPIC_SIVR	(lapic_addr + 0x0f0)
#परिभाषय LAPIC_ISR	(lapic_addr + 0x100)
#परिभाषय LAPIC_TMR	(lapic_addr + 0x180)
#परिभाषय LAPIC_IRR	(lapic_addr + 0x200)
#परिभाषय LAPIC_ESR	(lapic_addr + 0x280)
#परिभाषय LAPIC_ICR1	(lapic_addr + 0x300)
#परिभाषय LAPIC_ICR2	(lapic_addr + 0x310)
#परिभाषय LAPIC_LVTTR	(lapic_addr + 0x320)
#परिभाषय LAPIC_LVTTMR	(lapic_addr + 0x330)
#परिभाषय LAPIC_LVTPCR	(lapic_addr + 0x340)
#परिभाषय LAPIC_LINT0	(lapic_addr + 0x350)
#परिभाषय LAPIC_LINT1	(lapic_addr + 0x360)
#परिभाषय LAPIC_LVTER	(lapic_addr + 0x370)
#परिभाषय LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#परिभाषय LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#परिभाषय LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#परिभाषय IOAPIC_ID			0x0
#परिभाषय IOAPIC_संस्करणम्			0x1
#परिभाषय IOAPIC_ARB			0x2
#परिभाषय IOAPIC_REDIR_सारणी		0x10

#परिभाषय APIC_TIMER_INT_सदिशम्		0xf0
#परिभाषय APIC_SMP_SCHED_PROC_सदिशम्	0xf1
#परिभाषय APIC_SMP_CPU_HALT_सदिशम्	0xf2
#परिभाषय APIC_ERROR_INT_सदिशम्		0xfe
#परिभाषय APIC_SPURIOUS_INT_सदिशम्	0xff

#यदि_अपरिभाषितम् __ASSEMBLY__

#अन्तर्भाव "kernel/kernel.h"

EXTERN vir_अष्टकानि lapic_addr;
EXTERN vir_अष्टकानि lapic_eoi_addr;
EXTERN पूर्णाङ्क ioapic_enabled;
EXTERN पूर्णाङ्क bsp_lapic_id;

#परिभाषय MAX_NR_IOAPICS		32
#परिभाषय MAX_IOAPIC_IRQS		64

EXTERN पूर्णाङ्क ioapic_enabled;

संरचना io_apic {
	अचिह्नित	id;
	vir_अष्टकानि	addr; /* presently used address */
	phys_अष्टकानि	paddr; /* where is it in phys space */
	vir_अष्टकानि	vaddr; /* address after paging is on */
	अचिह्नित	pins;
	अचिह्नित	gsi_आधारः;
};

EXTERN संरचना io_apic io_apic[MAX_NR_IOAPICS];
EXTERN अचिह्नित nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

पूर्णाङ्क lapic_enable(अचिह्नित cpu);
शून्य ioapic_unmask_irq(अचिह्नित irq);
शून्य ioapic_आवरणम्_irq(अचिह्नित irq);
शून्य ioapic_reset_pic(शून्य);

EXTERN पूर्णाङ्क ioapic_enabled;
EXTERN अचिह्नित nioapics;

शून्य lapic_microsec_sleep(अचिह्नित गणनम्);
शून्य ioapic_disable_irqs(u32_t irqs);
शून्य ioapic_enable_irqs(u32_t irqs);

पूर्णाङ्क lapic_enable(अचिह्नित cpu);
शून्य lapic_disable(शून्य);

शून्य ioapic_disable_all(शून्य);
पूर्णाङ्क ioapic_enable_all(शून्य);

पूर्णाङ्क detect_ioapics(शून्य);
शून्य apic_idt_init(पूर्णाङ्क reset);

#यदि_परिभाषितम् CONFIG_SMP
पूर्णाङ्क apic_send_startup_ipi(अचिह्नित cpu, phys_अष्टकानि trampoline);
पूर्णाङ्क apic_send_init_ipi(अचिह्नित cpu, phys_अष्टकानि trampoline);
अचिह्नित पूर्णाङ्क apicid(शून्य);
शून्य ioapic_समुच्चयः_id(u32_t addr, अचिह्नित पूर्णाङ्क id);
#अन्यथा
पूर्णाङ्क apic_single_cpu_init(शून्य);
#यदि_अन्तः

शून्य lapic_समुच्चयः_timer_periodic(ध्रुव अचिह्नित freq);
शून्य lapic_समुच्चयः_timer_one_shot(ध्रुव u32_t मानम्);
शून्य lapic_stop_timer(शून्य);
शून्य lapic_restart_timer(शून्य);

शून्य ioapic_समुच्चयः_irq(अचिह्नित irq);
शून्य ioapic_unset_irq(अचिह्नित irq);

/* signal the end of interrupt handler to apic */
#परिभाषय apic_eoi() कुरु { *((चञ्चल u32_t *) lapic_eoi_addr) = 0; } यावत्(0)

शून्य ioapic_eoi(पूर्णाङ्क irq);

शून्य dump_apic_irq_state(शून्य);

शून्य apic_send_ipi(अचिह्नित सदिशम्, अचिह्नित cpu, पूर्णाङ्क type);

शून्य apic_ipi_sched_intr(शून्य);
शून्य apic_ipi_halt_intr(शून्य);

#परिभाषय APIC_IPI_DEST			0
#परिभाषय APIC_IPI_SELF			1
#परिभाषय APIC_IPI_गन्तव्यम्_ALL			2
#परिभाषय APIC_IPI_गन्तव्यम्_ALL_BUT_SELF	3

#परिभाषय apic_send_ipi_single(सदिशम्,cpu) \
	apic_send_ipi(सदिशम्, cpu, APIC_IPI_DEST);
#परिभाषय apic_send_ipi_self(सदिशम्) \
	apic_send_ipi(सदिशम्, 0, APIC_IPI_SELF)
#परिभाषय apic_send_ipi_all(सदिशम्) \
	apic_send_ipi (सदिशम्, 0, APIC_IPI_गन्तव्यम्_ALL)
#परिभाषय apic_send_ipi_allbutself(सदिशम्) \
	apic_send_ipi (सदिशम्, 0, APIC_IPI_गन्तव्यम्_ALL_BUT_SELF);


#अन्तर्भाव <minix/cpufeature.h>

#परिभाषय cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#परिभाषय lapic_read(what)	(*((चञ्चल u32_t *)((what))))
#परिभाषय lapic_write(what, दत्त)	कुरु {			\
	(*((चञ्चल u32_t *)((what)))) = दत्त;		\
} यावत्(0)

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* __APIC_H__ */
