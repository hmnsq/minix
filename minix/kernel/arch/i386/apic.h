#अगर_अघोषित __APIC_H__
#घोषणा __APIC_H__

#घोषणा APIC_ENABLE		0x100
#घोषणा APIC_FOCUS_DISABLED	(1 << 9)
#घोषणा APIC_SIV		0xFF

#घोषणा APIC_TDCR_2	0x00
#घोषणा APIC_TDCR_4	0x01
#घोषणा APIC_TDCR_8	0x02
#घोषणा APIC_TDCR_16	0x03
#घोषणा APIC_TDCR_32	0x08
#घोषणा APIC_TDCR_64	0x09
#घोषणा APIC_TDCR_128	0x0a
#घोषणा APIC_TDCR_1	0x0b

#घोषणा APIC_LVTT_VECTOR_MASK	0x000000FF
#घोषणा APIC_LVTT_DS_PENDING	(1 << 12)
#घोषणा APIC_LVTT_MASK		(1 << 16)
#घोषणा APIC_LVTT_TM		(1 << 17)

#घोषणा APIC_LVT_IIPP_MASK	0x00002000
#घोषणा APIC_LVT_IIPP_AH	0x00002000
#घोषणा APIC_LVT_IIPP_AL	0x00000000

#घोषणा IOAPIC_REGSEL		0x0
#घोषणा IOAPIC_RW		0x10

#घोषणा APIC_ICR_DM_MASK		0x00000700
#घोषणा APIC_ICR_VECTOR			APIC_LVTT_VECTOR_MASK
#घोषणा APIC_ICR_DM_FIXED		(0 << 8)
#घोषणा APIC_ICR_DM_LOWEST_PRIORITY	(1 << 8)
#घोषणा APIC_ICR_DM_SMI			(2 << 8)
#घोषणा APIC_ICR_DM_RESERVED		(3 << 8)
#घोषणा APIC_ICR_DM_NMI			(4 << 8)
#घोषणा APIC_ICR_DM_INIT		(5 << 8)
#घोषणा APIC_ICR_DM_STARTUP		(6 << 8)
#घोषणा APIC_ICR_DM_EXTINT		(7 << 8)

#घोषणा APIC_ICR_DM_PHYSICAL		(0 << 11)
#घोषणा APIC_ICR_DM_LOGICAL		(1 << 11)

#घोषणा APIC_ICR_DELIVERY_PENDING	(1 << 12)

#घोषणा APIC_ICR_INT_POLARITY		(1 << 13)

#घोषणा APIC_ICR_LEVEL_ASSERT		(1 << 14)
#घोषणा APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#घोषणा APIC_ICR_TRIGGER		(1 << 15)

#घोषणा APIC_ICR_INT_MASK		(1 << 16)

#घोषणा APIC_ICR_DEST_FIELD		(0 << 18)
#घोषणा APIC_ICR_DEST_SELF		(1 << 18)
#घोषणा APIC_ICR_DEST_ALL		(2 << 18)
#घोषणा APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#घोषणा LOCAL_APIC_DEF_ADDR	0xfee00000 /* default local apic address */
#घोषणा IO_APIC_DEF_ADDR	0xfec00000 /* default i/o apic address */

#घोषणा LAPIC_ID	(lapic_addr + 0x020)
#घोषणा LAPIC_VERSION	(lapic_addr + 0x030)
#घोषणा LAPIC_TPR	(lapic_addr + 0x080)
#घोषणा LAPIC_EOI	(lapic_addr + 0x0b0)
#घोषणा LAPIC_LDR	(lapic_addr + 0x0d0)
#घोषणा LAPIC_DFR	(lapic_addr + 0x0e0)
#घोषणा LAPIC_SIVR	(lapic_addr + 0x0f0)
#घोषणा LAPIC_ISR	(lapic_addr + 0x100)
#घोषणा LAPIC_TMR	(lapic_addr + 0x180)
#घोषणा LAPIC_IRR	(lapic_addr + 0x200)
#घोषणा LAPIC_ESR	(lapic_addr + 0x280)
#घोषणा LAPIC_ICR1	(lapic_addr + 0x300)
#घोषणा LAPIC_ICR2	(lapic_addr + 0x310)
#घोषणा LAPIC_LVTTR	(lapic_addr + 0x320)
#घोषणा LAPIC_LVTTMR	(lapic_addr + 0x330)
#घोषणा LAPIC_LVTPCR	(lapic_addr + 0x340)
#घोषणा LAPIC_LINT0	(lapic_addr + 0x350)
#घोषणा LAPIC_LINT1	(lapic_addr + 0x360)
#घोषणा LAPIC_LVTER	(lapic_addr + 0x370)
#घोषणा LAPIC_TIMER_ICR	(lapic_addr + 0x380)
#घोषणा LAPIC_TIMER_CCR	(lapic_addr + 0x390)
#घोषणा LAPIC_TIMER_DCR	(lapic_addr + 0x3e0)

#घोषणा IOAPIC_ID			0x0
#घोषणा IOAPIC_VERSION			0x1
#घोषणा IOAPIC_ARB			0x2
#घोषणा IOAPIC_REDIR_TABLE		0x10

#घोषणा APIC_TIMER_INT_VECTOR		0xf0
#घोषणा APIC_SMP_SCHED_PROC_VECTOR	0xf1
#घोषणा APIC_SMP_CPU_HALT_VECTOR	0xf2
#घोषणा APIC_ERROR_INT_VECTOR		0xfe
#घोषणा APIC_SPURIOUS_INT_VECTOR	0xff

#अगर_अघोषित __ASSEMBLY__

#समावेश "kernel/kernel.h"

EXTERN vir_bytes lapic_addr;
EXTERN vir_bytes lapic_eoi_addr;
EXTERN पूर्णांक ioapic_enabled;
EXTERN पूर्णांक bsp_lapic_id;

#घोषणा MAX_NR_IOAPICS		32
#घोषणा MAX_IOAPIC_IRQS		64

EXTERN पूर्णांक ioapic_enabled;

काष्ठा io_apic {
	अचिन्हित	id;
	vir_bytes	addr; /* presently used address */
	phys_bytes	paddr; /* where is it in phys space */
	vir_bytes	vaddr; /* address after paging is on */
	अचिन्हित	pins;
	अचिन्हित	gsi_base;
};

EXTERN काष्ठा io_apic io_apic[MAX_NR_IOAPICS];
EXTERN अचिन्हित nioapics;

EXTERN u32_t lapic_addr_vaddr; /* we remember the virtual address here until we
				  switch to paging */

पूर्णांक lapic_enable(अचिन्हित cpu);
व्योम ioapic_unmask_irq(अचिन्हित irq);
व्योम ioapic_mask_irq(अचिन्हित irq);
व्योम ioapic_reset_pic(व्योम);

EXTERN पूर्णांक ioapic_enabled;
EXTERN अचिन्हित nioapics;

व्योम lapic_microsec_sleep(अचिन्हित गिनती);
व्योम ioapic_disable_irqs(u32_t irqs);
व्योम ioapic_enable_irqs(u32_t irqs);

पूर्णांक lapic_enable(अचिन्हित cpu);
व्योम lapic_disable(व्योम);

व्योम ioapic_disable_all(व्योम);
पूर्णांक ioapic_enable_all(व्योम);

पूर्णांक detect_ioapics(व्योम);
व्योम apic_idt_init(पूर्णांक reset);

#अगर_घोषित CONFIG_SMP
पूर्णांक apic_send_startup_ipi(अचिन्हित cpu, phys_bytes trampoline);
पूर्णांक apic_send_init_ipi(अचिन्हित cpu, phys_bytes trampoline);
अचिन्हित पूर्णांक apicid(व्योम);
व्योम ioapic_set_id(u32_t addr, अचिन्हित पूर्णांक id);
#अन्यथा
पूर्णांक apic_single_cpu_init(व्योम);
#पूर्ण_अगर

व्योम lapic_set_timer_periodic(स्थिर अचिन्हित freq);
व्योम lapic_set_timer_one_shot(स्थिर u32_t मूल्य);
व्योम lapic_stop_timer(व्योम);
व्योम lapic_restart_timer(व्योम);

व्योम ioapic_set_irq(अचिन्हित irq);
व्योम ioapic_unset_irq(अचिन्हित irq);

/* signal the end of interrupt handler to apic */
#घोषणा apic_eoi() करो { *((अस्थिर u32_t *) lapic_eoi_addr) = 0; } जबतक(0)

व्योम ioapic_eoi(पूर्णांक irq);

व्योम dump_apic_irq_state(व्योम);

व्योम apic_send_ipi(अचिन्हित vector, अचिन्हित cpu, पूर्णांक type);

व्योम apic_ipi_sched_intr(व्योम);
व्योम apic_ipi_halt_intr(व्योम);

#घोषणा APIC_IPI_DEST			0
#घोषणा APIC_IPI_SELF			1
#घोषणा APIC_IPI_TO_ALL			2
#घोषणा APIC_IPI_TO_ALL_BUT_SELF	3

#घोषणा apic_send_ipi_single(vector,cpu) \
	apic_send_ipi(vector, cpu, APIC_IPI_DEST);
#घोषणा apic_send_ipi_self(vector) \
	apic_send_ipi(vector, 0, APIC_IPI_SELF)
#घोषणा apic_send_ipi_all(vector) \
	apic_send_ipi (vector, 0, APIC_IPI_TO_ALL)
#घोषणा apic_send_ipi_allbutself(vector) \
	apic_send_ipi (vector, 0, APIC_IPI_TO_ALL_BUT_SELF);


#समावेश <minix/cpufeature.h>

#घोषणा cpu_feature_apic_on_chip() _cpufeature(_CPUF_I386_APIC_ON_CHIP)

#घोषणा lapic_read(what)	(*((अस्थिर u32_t *)((what))))
#घोषणा lapic_write(what, तथ्य)	करो {			\
	(*((अस्थिर u32_t *)((what)))) = तथ्य;		\
} जबतक(0)

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* __APIC_H__ */
