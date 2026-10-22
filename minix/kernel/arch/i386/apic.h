#నిర్వచితం_కాకపోతే __APIC_H__
#నిర్వచించు __APIC_H__

#నిర్వచించు APIC_ENABLE		0x100
#నిర్వచించు APIC_FOCUS_DISABLED	(1 << 9)
#నిర్వచించు APIC_SIV		0xFF

#నిర్వచించు APIC_TDCR_2	0x00
#నిర్వచించు APIC_TDCR_4	0x01
#నిర్వచించు APIC_TDCR_8	0x02
#నిర్వచించు APIC_TDCR_16	0x03
#నిర్వచించు APIC_TDCR_32	0x08
#నిర్వచించు APIC_TDCR_64	0x09
#నిర్వచించు APIC_TDCR_128	0x0a
#నిర్వచించు APIC_TDCR_1	0x0b

#నిర్వచించు APIC_LVTT_సదిశ_ముసుగు	0x000000FF
#నిర్వచించు APIC_LVTT_DS_PENDING	(1 << 12)
#నిర్వచించు APIC_LVTT_ముసుగు		(1 << 16)
#నిర్వచించు APIC_LVTT_TM		(1 << 17)

#నిర్వచించు APIC_LVT_IIPP_ముసుగు	0x00002000
#నిర్వచించు APIC_LVT_IIPP_AH	0x00002000
#నిర్వచించు APIC_LVT_IIPP_AL	0x00000000

#నిర్వచించు IOAPIC_REGSEL		0x0
#నిర్వచించు IOAPIC_RW		0x10

#నిర్వచించు APIC_ICR_DM_ముసుగు		0x00000700
#నిర్వచించు APIC_ICR_సదిశ			APIC_LVTT_సదిశ_ముసుగు
#నిర్వచించు APIC_ICR_DM_FIXED		(0 << 8)
#నిర్వచించు APIC_ICR_DM_అత్యల్ప_ప్రాధాన్యత	(1 << 8)
#నిర్వచించు APIC_ICR_DM_SMI			(2 << 8)
#నిర్వచించు APIC_ICR_DM_RESERVED		(3 << 8)
#నిర్వచించు APIC_ICR_DM_NMI			(4 << 8)
#నిర్వచించు APIC_ICR_DM_INIT		(5 << 8)
#నిర్వచించు APIC_ICR_DM_STARTUP		(6 << 8)
#నిర్వచించు APIC_ICR_DM_EXTINT		(7 << 8)

#నిర్వచించు APIC_ICR_DM_PHYSICAL		(0 << 11)
#నిర్వచించు APIC_ICR_DM_LOGICAL		(1 << 11)

#నిర్వచించు APIC_ICR_DELIVERY_PENDING	(1 << 12)

#నిర్వచించు APIC_ICR_INT_POLARITY		(1 << 13)

#నిర్వచించు APIC_ICR_LEVEL_ASSERT		(1 << 14)
#నిర్వచించు APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#నిర్వచించు APIC_ICR_TRIGGER		(1 << 15)

#నిర్వచించు APIC_ICR_INT_ముసుగు		(1 << 16)

#నిర్వచించు APIC_ICR_DEST_క్షేత్రం		(0 << 18)
#నిర్వచించు APIC_ICR_DEST_SELF		(1 << 18)
#నిర్వచించు APIC_ICR_DEST_ALL		(2 << 18)
#నిర్వచించు APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#నిర్వచించు LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#నిర్వచించు IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#నిర్వచించు LAPIC_ID	(lapic_addr + 0x020)
#నిర్వచించు LAPIC_సంస్కరణ	(lapic_addr + 0x030)
#నిర్వచించు LAPIC_TPR	(lapic_addr + 0x080)
#నిర్వచించు LAPIC_EOI	(lapic_addr + 0x0b0)
#నిర్వచించు LAPIC_LDR	(lapic_addr + 0x0d0)
#నిర్వచించు LAPIC_DFR	(lapic_addr + 0x0e0)
#నిర్వచించు LAPIC_SIVR	(lapic_addr + 0x0f0)
#నిర్వచించు LAPIC_ISR	(lapic_addr + 0x100)
#నిర్వచించు LAPIC_TMR	(lapic_addr + 0x180)
#నిర్వచించు LAPIC_IRR	(lapic_addr + 0x200)
#నిర్వచించు LAPIC_ESR	(lapic_addr + 0x280)
#నిర్వచించు LAPIC_ICR1	(lapic_addr + 0x300)
#నిర్వచించు LAPIC_ICR2	(lapic_addr + 0x310)
#నిర్వచించు LAPIC_LVTTR	(lapic_addr + 0x320)
#నిర్వచించు LAPIC_LVTTMR	(lapic_addr + 0x330)
#నిర్వచించు LAPIC_LVTPCR	(lapic_addr + 0x340)
#నిర్వచించు LAPIC_LINT0	(lapic_addr + 0x350)
#నిర్వచించు LAPIC_LINT1	(lapic_addr + 0x360)
#నిర్వచించు LAPIC_LVTER	(lapic_addr + 0x370)
#నిర్వచించు LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#నిర్వచించు LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#నిర్వచించు LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#నిర్వచించు IOAPIC_ID			0x0
#నిర్వచించు IOAPIC_సంస్కరణ			0x1
#నిర్వచించు IOAPIC_ARB			0x2
#నిర్వచించు IOAPIC_REDIR_పట్టిక		0x10

#నిర్వచించు APIC_TIMER_INT_సదిశ		0xf0
#నిర్వచించు APIC_SMP_SCHED_PROC_సదిశ	0xf1
#నిర్వచించు APIC_SMP_CPU_HALT_సదిశ	0xf2
#నిర్వచించు APIC_ERROR_INT_సదిశ		0xfe
#నిర్వచించు APIC_SPURIOUS_INT_సదిశ	0xff

#నిర్వచితం_కాకపోతే __ASSEMBLY__

#అన్తర్భావ "kernel/kernel.h"

EXTERN vir_బైట్లు lapic_addr;
EXTERN vir_బైట్లు lapic_eoi_addr;
EXTERN పూర్ణాంకం ioapic_enabled;
EXTERN పూర్ణాంకం bsp_lapic_id;

#నిర్వచించు MAX_NR_IOAPICS		32
#నిర్వచించు MAX_IOAPIC_IRQS		64

EXTERN పూర్ణాంకం ioapic_enabled;

నిర్మాణం io_apic {
	గుర్తులేని	id;
	vir_బైట్లు	addr; /* presently used address */
	phys_బైట్లు	paddr; /* where is it in phys space */
	vir_బైట్లు	vaddr; /* address after paging is on */
	గుర్తులేని	pins;
	గుర్తులేని	gsi_పునాది;
};

EXTERN నిర్మాణం io_apic io_apic[MAX_NR_IOAPICS];
EXTERN గుర్తులేని nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

పూర్ణాంకం lapic_enable(గుర్తులేని cpu);
శూన్య ioapic_unmask_irq(గుర్తులేని irq);
శూన్య ioapic_ముసుగు_irq(గుర్తులేని irq);
శూన్య ioapic_reset_pic(శూన్య);

EXTERN పూర్ణాంకం ioapic_enabled;
EXTERN గుర్తులేని nioapics;

శూన్య lapic_microsec_sleep(గుర్తులేని లెక్క);
శూన్య ioapic_disable_irqs(u32_t irqs);
శూన్య ioapic_enable_irqs(u32_t irqs);

పూర్ణాంకం lapic_enable(గుర్తులేని cpu);
శూన్య lapic_disable(శూన్య);

శూన్య ioapic_disable_all(శూన్య);
పూర్ణాంకం ioapic_enable_all(శూన్య);

పూర్ణాంకం detect_ioapics(శూన్య);
శూన్య apic_idt_init(పూర్ణాంకం reset);

#నిర్వచితమైతే CONFIG_SMP
పూర్ణాంకం apic_send_startup_ipi(గుర్తులేని cpu, phys_బైట్లు trampoline);
పూర్ణాంకం apic_send_init_ipi(గుర్తులేని cpu, phys_బైట్లు trampoline);
గుర్తులేని పూర్ణాంకం apicid(శూన్య);
శూన్య ioapic_సమితి_id(u32_t addr, గుర్తులేని పూర్ణాంకం id);
#లేకపోతే
పూర్ణాంకం apic_single_cpu_init(శూన్య);
#అయితే_ముగింపు

శూన్య lapic_సమితి_timer_periodic(స్థిరరాశి గుర్తులేని freq);
శూన్య lapic_సమితి_timer_one_shot(స్థిరరాశి u32_t విలువ);
శూన్య lapic_stop_timer(శూన్య);
శూన్య lapic_restart_timer(శూన్య);

శూన్య ioapic_సమితి_irq(గుర్తులేని irq);
శూన్య ioapic_unset_irq(గుర్తులేని irq);

/* signal the end of interrupt handler to apic */
#నిర్వచించు apic_eoi() చేయి { *((అస్థిర u32_t *) lapic_eoi_addr) = 0; } అయితే(0)

శూన్య ioapic_eoi(పూర్ణాంకం irq);

శూన్య dump_apic_irq_state(శూన్య);

శూన్య apic_send_ipi(గుర్తులేని సదిశ, గుర్తులేని cpu, పూర్ణాంకం type);

శూన్య apic_ipi_sched_intr(శూన్య);
శూన్య apic_ipi_halt_intr(శూన్య);

#నిర్వచించు APIC_IPI_DEST			0
#నిర్వచించు APIC_IPI_SELF			1
#నిర్వచించు APIC_IPI_గమ్యం_ALL			2
#నిర్వచించు APIC_IPI_గమ్యం_ALL_BUT_SELF	3

#నిర్వచించు apic_send_ipi_single(సదిశ,cpu) \
	apic_send_ipi(సదిశ, cpu, APIC_IPI_DEST);
#నిర్వచించు apic_send_ipi_self(సదిశ) \
	apic_send_ipi(సదిశ, 0, APIC_IPI_SELF)
#నిర్వచించు apic_send_ipi_all(సదిశ) \
	apic_send_ipi (సదిశ, 0, APIC_IPI_గమ్యం_ALL)
#నిర్వచించు apic_send_ipi_allbutself(సదిశ) \
	apic_send_ipi (సదిశ, 0, APIC_IPI_గమ్యం_ALL_BUT_SELF);


#అన్తర్భావ <minix/cpufeature.h>

#నిర్వచించు cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#నిర్వచించు lapic_read(what)	(*((అస్థిర u32_t *)((what))))
#నిర్వచించు lapic_write(what, దత్తాంశం)	చేయి {			\
	(*((అస్థిర u32_t *)((what)))) = దత్తాంశం;		\
} అయితే(0)

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* __APIC_H__ */
