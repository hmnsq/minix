/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/portio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/syslib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cmos.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "apic_asm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw_intr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "acpi.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/watchdog.h"
#ਜੇ_ਅੰਤ

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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਸੈੱਟ(ਨਕਾਬ)		(ਨਕਾਬ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_CLEAR(ਨਕਾਬ)		0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_ਸਦਿਸ਼_ਨਕਾਬ	0x000000FF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_DS_PENDING	(1 << 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_ਨਕਾਬ		(1 << 16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVTT_TM		(1 << 17)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_ਨਕਾਬ	0x00002000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_AH	0x00002000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_IIPP_AL	0x00000000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_LVT_TM_PERIODIC	IS_ਸੈੱਟ(APIC_LVTT_TM)

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_INTPOL_ਨੀਵਾਂ		IS_ਸੈੱਟ(APIC_ICR_INT_POLARITY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_INTPOL_ਉੱਚਾ		IS_CLEAR(APIC_ICR_INT_POLARITY)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_LEVEL_ASSERT		(1 << 14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_TRIGGER		(1 << 15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_TM_ਕਿਨਾਰਾ		IS_CLEAR(APIC_ICR_TRIGGER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_INT_ਨਕਾਬ		(1 << 16)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ਖੇਤਰ		(0 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_SELF		(1 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ALL		(2 << 18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IA32_APIC_ਅਧਾਰ	0x1b
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IA32_APIC_ਅਧਾਰ_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LAPIC_ਸਦਿਸ਼(irq)	(IRQ0_ਸਦਿਸ਼ +(irq))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPL0				0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	SPLHI				0xF


ਬਣਤਰ io_apic io_apic[MAX_NR_IOAPICS];
ਬਿਨਾਨਿਸ਼ਾਨੀ nioapics;

ਬਣਤਰ irq;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (* eoi_ਵਿਧੀ_t)(ਬਣਤਰ irq *);

ਬਣਤਰ irq {
	ਬਣਤਰ io_apic * 	ioa;
	ਬਿਨਾਨਿਸ਼ਾਨੀ		pin;
	ਬਿਨਾਨਿਸ਼ਾਨੀ		ਸਦਿਸ਼;
	eoi_ਵਿਧੀ_t		eoi;
	ਬਿਨਾਨਿਸ਼ਾਨੀ		state;
};

ਸਥਿਰ ਬਣਤਰ irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/smp.h"

#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/spinlock.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERBOSE_APIC(x) x

ਪੂਰਨ ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_ਬਾਈਟਾਂ lapic_addr;
vir_ਬਾਈਟਾਂ lapic_eoi_addr;
ਪੂਰਨ bsp_lapic_id;

ਸਥਿਰ ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ probe_ticks;
ਸਥਿਰ	u64_t tsc0, tsc1;
ਸਥਿਰ	u32_t lapic_tctr0, lapic_tctr1;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ apic_imcrp;
ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ nlints = 0;

ਖਾਲੀ arch_eoi(ਖਾਲੀ)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
ਸਥਿਰ u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROBE_TICKS	(system_hz / 10)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_IOREGSEL	0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_IOWIN	0x10

ਸਥਿਰ u32_t ioapic_read(u32_t ioa_ਅਧਾਰ, u32_t reg)
{
	*((ਅਸਥਿਰ u32_t *)(ioa_ਅਧਾਰ + IOAPIC_IOREGSEL)) = (reg & 0xff);
	ਵਾਪਸ *(ਅਸਥਿਰ u32_t *)(ioa_ਅਧਾਰ + IOAPIC_IOWIN);
}

ਸਥਿਰ ਖਾਲੀ ioapic_write(u32_t ioa_ਅਧਾਰ, u8_t reg, u32_t val)
{
	*((ਅਸਥਿਰ u32_t *)(ioa_ਅਧਾਰ + IOAPIC_IOREGSEL)) = reg;
	*((ਅਸਥਿਰ u32_t *)(ioa_ਅਧਾਰ + IOAPIC_IOWIN)) = val;
}

ਖਾਲੀ lapic_microsec_sleep(ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2);
ਖਾਲੀ apic_idt_init(ਅਚਲ ਪੂਰਨ reset);

ਸਥਿਰ ਖਾਲੀ ioapic_enable_pin(vir_ਬਾਈਟਾਂ ioapic_addr, ਪੂਰਨ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_ਸਾਰਣੀ + pin * 2);

	lo &= ~APIC_ICR_INT_ਨਕਾਬ;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_ਸਾਰਣੀ + pin * 2, lo);
}

ਸਥਿਰ ਖਾਲੀ ioapic_disable_pin(vir_ਬਾਈਟਾਂ ioapic_addr, ਪੂਰਨ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_ਸਾਰਣੀ + pin * 2);

	lo |= APIC_ICR_INT_ਨਕਾਬ;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_ਸਾਰਣੀ + pin * 2, lo);
}

#ਜੇ 0
ਸਥਿਰ ਖਾਲੀ ioapic_redirt_ਇੰਦਰਾਜ_read(ਖਾਲੀ * ioapic_addr,
					ਪੂਰਨ ਇੰਦਰਾਜ,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + ਇੰਦਰਾਜ * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + ਇੰਦਰਾਜ * 2 + 1));

}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ ioapic_redirt_ਇੰਦਰਾਜ_write(ਖਾਲੀ * ioapic_addr,
					ਪੂਰਨ ਇੰਦਰਾਜ,
					u32_t hi,
					u32_t lo)
{
#ਜੇ 0
	VERBOSE_APIC(ਲਿਖੋ("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", ਇੰਦਰਾਜ, hi, lo));
#ਜੇ_ਅੰਤ
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + ਇੰਦਰਾਜ * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + ਇੰਦਰਾਜ * 2), lo);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_read_tmr_ਸਦਿਸ਼(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_read_irr_ਸਦਿਸ਼(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ apic_read_isr_ਸਦਿਸ਼(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ lapic_test_delivery_val(val, ਸਦਿਸ਼) ((val) & (1 << ((ਸਦਿਸ਼) & 0x1f)))

ਸਥਿਰ ਖਾਲੀ ioapic_eoi_level(ਬਣਤਰ irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_ਸਦਿਸ਼(irq->ਸਦਿਸ਼);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	ਜੇ (!lapic_test_delivery_val(tmr, irq->ਸਦਿਸ਼)) {
		ਪੂਰਨ is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_ਸਾਰਣੀ + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_ਨਕਾਬ;

		/* set mask and edge */
		lo |= APIC_ICR_INT_ਨਕਾਬ;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_ਸਾਰਣੀ + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_ਸਾਰਣੀ + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		ਜੇ (is_masked)
			lo |= APIC_ICR_INT_ਨਕਾਬ;
		ਨਹੀਂਤਾਂ
			lo &= ~APIC_ICR_INT_ਨਕਾਬ;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_ਸਾਰਣੀ + irq->pin * 2, lo);
	}
}

ਸਥਿਰ ਖਾਲੀ ioapic_eoi_ਕਿਨਾਰਾ(__unused ਬਣਤਰ irq * irq)
{
	apic_eoi();
}

ਖਾਲੀ ioapic_eoi(ਪੂਰਨ irq)
{
	ਜੇ (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	ਨਹੀਂਤਾਂ
		irq_8259_eoi(irq);
}
 
ਖਾਲੀ ioapic_ਸੈੱਟ_id(u32_t addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

ਪੂਰਨ ioapic_enable_all(ਖਾਲੀ)
{
	i8259_disable();

	ਜੇ (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	ਵਾਪਸ ioapic_enabled = 1;
}

/* disables a single IO APIC */
ਸਥਿਰ ਖਾਲੀ ioapic_disable(ਬਣਤਰ io_apic * ioapic)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ p;
	
	ਲਈ (p = 0; p < io_apic->pins; p++) {
		u32_t ਨੀਵਾਂ_32, hi_32;
		ਨੀਵਾਂ_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + p * 2 + 1));

		ਜੇ (!(ਨੀਵਾਂ_32 & APIC_ICR_INT_ਨਕਾਬ)) {
			ਨੀਵਾਂ_32 |= APIC_ICR_INT_ਨਕਾਬ;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ਸਾਰਣੀ + p * 2), ਨੀਵਾਂ_32);
		}
	}
}

/* disables all IO APICs */
ਖਾਲੀ ioapic_disable_all(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ioa;
	ਜੇ (!ioapic_enabled)
		ਵਾਪਸ;

	ਲਈ (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	ਜੇ (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

ਸਥਿਰ ਖਾਲੀ ioapic_disable_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਜੇ(!(io_apic_irq[irq].ioa)) {
		ਲਿਖੋ("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		ਵਾਪਸ;
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

ਸਥਿਰ ਖਾਲੀ ioapic_enable_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਜੇ(!(io_apic_irq[irq].ioa)) {
		ਲਿਖੋ("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		ਵਾਪਸ;
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

ਖਾਲੀ ioapic_unmask_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਜੇ (ioapic_enabled)
		ioapic_enable_irq(irq);
	ਨਹੀਂਤਾਂ
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

ਖਾਲੀ ioapic_ਨਕਾਬ_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਜੇ (ioapic_enabled)
		ioapic_disable_irq(irq);
	ਨਹੀਂਤਾਂ
		/* FIXME unlikely */
		irq_8259_ਨਕਾਬ(irq);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ apicid(ਖਾਲੀ)
{
	ਵਾਪਸ lapic_read(LAPIC_ID) >> 24;
}

ਸਥਿਰ ਪੂਰਨ calib_clk_handler(irq_hook_t * ਅਣਵਰਤਿਆ(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	ਜੇ (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	ਨਹੀਂਤਾਂ ਜੇ (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ spurious_irq_handler(irq_hook_t * ਅਣਵਰਤਿਆ(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ apic_calibrate_clocks(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(ਲਿਖੋ("Calibrating clock\n"));
	/*
	 * Set Initial count register to the highest value so it does not
	 * underflow during the testing period
	 * */
	val = 0xffffffff;
	lapic_write (LAPIC_TIMER_ICR, val);

	/* Set Current count register */
	val = 0;
	lapic_write (LAPIC_TIMER_CCR, val);

	lvtt = lapic_read(LAPIC_TIMER_DCR) & ~0x0b;
	 /* Set Divide configuration register to 1 */
	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/*
	 * mask the APIC timer interrupt in the LVT Timer Register so that we
	 * don't get an interrupt upon underflow which we don't know how to
	 * handle right know. If underflow happens, the system will not continue
	 * as something is wrong with the clock IRQ 0 and we cannot calibrate
	 * the clock which mean that we cannot run processes
	 */
	lvtt = lapic_read (LAPIC_LVTTR);
	lvtt |= APIC_LVTT_ਨਕਾਬ;
	lapic_write (LAPIC_LVTTR, lvtt);

	/* set the probe, we use the legacy timer, IRQ 0 */
	put_irq_handler(&calib_clk, CLOCK_IRQ, calib_clk_handler);

	/*
	 * A spurious interrupt may occur during the clock calibration. Since we
	 * do this calibration in kernel, we need a special handler which will
	 * leave the BKL unlocked like the clock handler. This is a corner case,
	 * boot time only situation
	 */
	put_irq_handler(&spurious_irq, SPURIOUS_IRQ, spurious_irq_handler);

	/* set the PIC timer to get some time */
	init_8253A_timer(system_hz);

	/*
	 * We must unlock BKL here as the in-kernel interrupt will lock it
	 * again. The handler will unlock it after it is done. This is
	 * absolutely safe as only the BSP is running. It is just a workaround a
	 * corner case for APIC timer calibration
	 */
	BKL_UNLOCK();
	intr_enable();

	/* loop for some time to get a sample */
	ਜਦੋਂ(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_ਤਾਲਾ();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(ਲਿਖੋ("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_ਸੈੱਟ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

ਖਾਲੀ lapic_ਸੈੱਟ_timer_one_shot(ਅਚਲ u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_ਪ੍ਰਤੀ_us;
	ਅਚਲ u8_t cpu = cpuid;

	ticks_ਪ੍ਰਤੀ_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_ਪ੍ਰਤੀ_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_ਸਦਿਸ਼;
	lapic_write(LAPIC_LVTTR, lvtt);
}

ਖਾਲੀ lapic_ਸੈੱਟ_timer_periodic(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_ਪ੍ਰਤੀ_clock_tick;
	ਅਚਲ u8_t cpu = cpuid;

	lapic_ticks_ਪ੍ਰਤੀ_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_ਸਦਿਸ਼;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_ਪ੍ਰਤੀ_clock_tick);
}

ਖਾਲੀ lapic_stop_timer(ਖਾਲੀ)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_ਨਕਾਬ);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

ਖਾਲੀ lapic_restart_timer(ਖਾਲੀ)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	ਜੇ (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_ਸੈੱਟ_timer_one_shot(1000000/system_hz);
}

ਖਾਲੀ lapic_microsec_sleep(ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2)
{
	lapic_ਸੈੱਟ_timer_one_shot(ਗਿਣਤੀ_2);
	ਜਦੋਂ (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

ਸਥਿਰ  u32_t lapic_errstatus(ਖਾਲੀ)
{
	lapic_write(LAPIC_ESR, 0);
	ਵਾਪਸ lapic_read(LAPIC_ESR);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਪੂਰਨ lapic_disable_ਵਿੱਚ_msr(ਖਾਲੀ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_ਅਧਾਰ, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_ਅਧਾਰ_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
	ia32_msr_write(IA32_APIC_ਅਧਾਰ, msr_hi, msr_lo);

	ਵਾਪਸ 1;
}
#ਜੇ_ਅੰਤ /* CONFIG_SMP */

ਖਾਲੀ lapic_disable(ਖਾਲੀ)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	ਜੇ (!lapic_addr)
		ਵਾਪਸ;
	
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ (cpu_is_bsp(cpuid) && !apic_imcrp)
#ਜੇ_ਅੰਤ
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_ਨਕਾਬ|APIC_ICR_INT_ਨਕਾਬ);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		ਵਾਪਸ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ਨਕਾਬ;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ਨਕਾਬ;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_ਵਿੱਚ_msr();
#ਜੇ_ਅੰਤ /* CONFIG_SMP */
}

ਸਥਿਰ ਪੂਰਨ lapic_enable_ਵਿੱਚ_msr(ਖਾਲੀ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_ਅਧਾਰ, &msr_hi, &msr_lo);

#ਜੇ 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	ਜੇ (addr != (lapic_addr >> 12)) {
		ਜੇ (msr_hi & 0xf) {
			ਲਿਖੋ("ERROR : APIC address needs more then 32 bits\n");
			ਵਾਪਸ 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#ਜੇ_ਅੰਤ

	msr_lo |= (1 << IA32_APIC_ਅਧਾਰ_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
	ia32_msr_write(IA32_APIC_ਅਧਾਰ, msr_hi, msr_lo);

	ਵਾਪਸ 1;
}

ਪੂਰਨ lapic_enable(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	u32_t val, nlvt;

	ਜੇ (!lapic_addr)
		ਵਾਪਸ 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	ਜੇ (!cpu_has_tsc) {
		ਲਿਖੋ("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		ਵਾਪਸ 0;
	}

	ਜੇ (!lapic_enable_ਵਿੱਚ_msr())
		ਵਾਪਸ 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_ਸਦਿਸ਼;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(ਖਾਲੀ) lapic_read(LAPIC_SIVR);

	apic_eoi();

	/* Program Logical Destination Register. */
	val = lapic_read(LAPIC_LDR) & ~0xFF000000;
	val |= (cpu & 0xFF) << 24;
	lapic_write(LAPIC_LDR, val);

	/* Program Destination Format Register for Flat mode. */
	val = lapic_read(LAPIC_DFR) | 0xF0000000;
	lapic_write (LAPIC_DFR, val);

	val = lapic_read (LAPIC_LVTER) & 0xFFFFFF00;
	lapic_write (LAPIC_LVTER, val);

	nlvt = (lapic_read(LAPIC_ਸੰਸਕਰਣ)>>16) & 0xFF;

	ਜੇ(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_ਨਕਾਬ);
	}

	ਜੇ(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_ਨਕਾਬ);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(ਖਾਲੀ) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(ਲਿਖੋ("APIC timer calibrated\n"));

	ਵਾਪਸ 1;
}

ਖਾਲੀ apic_spurios_intr_handler(ਖਾਲੀ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ x;

	x++;
	ਜੇ (x == 1 || (x % 100) == 0)
		ਲਿਖੋ("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

ਖਾਲੀ apic_error_intr_handler(ਖਾਲੀ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ x;

	x++;
	ਜੇ (x == 1 || (x % 100) == 0)
		ਲਿਖੋ("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

ਸਥਿਰ ਬਣਤਰ gate_ਸਾਰਣੀ_s gate_ਸਾਰਣੀ_ioapic[] = {
	{ apic_hwint0, LAPIC_ਸਦਿਸ਼( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_ਸਦਿਸ਼( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_ਸਦਿਸ਼( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_ਸਦਿਸ਼( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_ਸਦਿਸ਼( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_ਸਦਿਸ਼( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_ਸਦਿਸ਼( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_ਸਦਿਸ਼( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_ਸਦਿਸ਼( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_ਸਦਿਸ਼( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_ਸਦਿਸ਼(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_ਸਦਿਸ਼(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_ਸਦਿਸ਼(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_ਸਦਿਸ਼(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_ਸਦਿਸ਼(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_ਸਦਿਸ਼(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_ਸਦਿਸ਼(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_ਸਦਿਸ਼(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_ਸਦਿਸ਼(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_ਸਦਿਸ਼(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_ਸਦਿਸ਼(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_ਸਦਿਸ਼(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_ਸਦਿਸ਼(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_ਸਦਿਸ਼(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_ਸਦਿਸ਼(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_ਸਦਿਸ਼(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_ਸਦਿਸ਼(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_ਸਦਿਸ਼(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_ਸਦਿਸ਼(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_ਸਦਿਸ਼(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_ਸਦਿਸ਼(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_ਸਦਿਸ਼(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_ਸਦਿਸ਼(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_ਸਦਿਸ਼(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_ਸਦਿਸ਼(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_ਸਦਿਸ਼(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_ਸਦਿਸ਼(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_ਸਦਿਸ਼(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_ਸਦਿਸ਼(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_ਸਦਿਸ਼(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_ਸਦਿਸ਼(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_ਸਦਿਸ਼(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_ਸਦਿਸ਼(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_ਸਦਿਸ਼(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_ਸਦਿਸ਼(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_ਸਦਿਸ਼(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_ਸਦਿਸ਼(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_ਸਦਿਸ਼(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_ਸਦਿਸ਼(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_ਸਦਿਸ਼(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_ਸਦਿਸ਼(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_ਸਦਿਸ਼(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_ਸਦਿਸ਼(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_ਸਦਿਸ਼(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_ਸਦਿਸ਼(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_ਸਦਿਸ਼(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_ਸਦਿਸ਼(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_ਸਦਿਸ਼(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_ਸਦਿਸ਼(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_ਸਦਿਸ਼(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_ਸਦਿਸ਼(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_ਸਦਿਸ਼(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_ਸਦਿਸ਼(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_ਸਦਿਸ਼(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ ਨਿਰਾ, 0, 0}
};

ਸਥਿਰ ਬਣਤਰ gate_ਸਾਰਣੀ_s gate_ਸਾਰਣੀ_common[] = {
	{ ipc_ਇੰਦਰਾਜ_softint_orig, IPC_ਸਦਿਸ਼_ORIG, USER_PRIVILEGE },
	{ kernel_call_ਇੰਦਰਾਜ_orig, KERN_CALL_ਸਦਿਸ਼_ORIG, USER_PRIVILEGE },
	{ ipc_ਇੰਦਰਾਜ_softint_um, IPC_ਸਦਿਸ਼_UM, USER_PRIVILEGE },
	{ kernel_call_ਇੰਦਰਾਜ_um, KERN_CALL_ਸਦਿਸ਼_UM, USER_PRIVILEGE },
	{ ਨਿਰਾ, 0, 0}
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਸਥਿਰ ਬਣਤਰ gate_ਸਾਰਣੀ_s gate_ਸਾਰਣੀ_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ ਨਿਰਾ, 0, 0}
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ APIC_DEBUG
ਸਥਿਰ ਖਾਲੀ lapic_ਸੈੱਟ_dummy_handlers(ਖਾਲੀ)
{
	ਅੱਖਰ * handler;
	ਪੂਰਨ vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_ਸ਼ੁਰੂ;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_ਮਾਪ;
	ਲਈ(; handler < &lapic_intr_dummy_handles_ਸਿਰਾ;
			handler += LAPIC_INTR_DUMMY_HANDLER_ਮਾਪ) {
		int_gate_idt(vect++, (vir_ਬਾਈਟਾਂ) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_ਖਿਸਕਾਓ));
	}
}
#ਜੇ_ਅੰਤ

/* Build descriptors for interrupt gates in IDT. */
ਖਾਲੀ apic_idt_init(ਅਚਲ ਪੂਰਨ reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	ਪੂਰਨ is_bsp;

	ਜੇ (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_ਸਾਰਣੀ_common);
		ਵਾਪਸ;
	}

	is_bsp = is_boot_apic(apicid());

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ APIC_DEBUG
	ਜੇ (is_bsp)
		ਲਿਖੋ("APIC debugging is enabled\n");
	lapic_ਸੈੱਟ_dummy_handlers();
#ਜੇ_ਅੰਤ

	/* Build descriptors for interrupt gates in IDT. */
	ਜੇ (ioapic_enabled)
		idt_copy_vectors(gate_ਸਾਰਣੀ_ioapic);
	ਨਹੀਂਤਾਂ
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_ਸਾਰਣੀ_common);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	idt_copy_vectors(gate_ਸਾਰਣੀ_smp);
#ਜੇ_ਅੰਤ

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_ਸਦਿਸ਼;
	val &= ~ APIC_ICR_INT_ਨਕਾਬ;
	lapic_write(LAPIC_LVTER, val);
	(ਖਾਲੀ) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	ਜੇ (is_bsp) {
		BOOT_VERBOSE(ਲਿਖੋ("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_ਸਦਿਸ਼, (vir_ਬਾਈਟਾਂ) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_ਖਿਸਕਾਓ));
	}

}

ਸਥਿਰ ਪੂਰਨ acpi_get_ioapics(ਬਣਤਰ io_apic * ioa, ਬਿਨਾਨਿਸ਼ਾਨੀ * nioa, ਬਿਨਾਨਿਸ਼ਾਨੀ max)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0;
	ਬਣਤਰ acpi_madt_ioapic * acpi_ioa;

	ਜਦੋਂ (n < max) {
		acpi_ioa = acpi_get_ioapic_ਅਗਲਾ();
		ਜੇ (acpi_ioa == ਨਿਰਾ)
			ਰੋਕੋ;

		ਪ੍ਰਤਿਜ੍ਞਾ(acpi_ioa->ਪਤਾ);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->ਪਤਾ;
		ioa[n].paddr = (phys_ਬਾਈਟਾਂ) acpi_ioa->ਪਤਾ;
		ioa[n].gsi_ਅਧਾਰ = acpi_ioa->global_int_ਅਧਾਰ;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_ਸੰਸਕਰਣ) & 0xff0000) >> 16)+1;
		ਲਿਖੋ("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	ਵਾਪਸ n;
}

ਪੂਰਨ detect_ioapics(ਖਾਲੀ)
{
	ਪੂਰਨ ਸਥਿਤੀ;

	ਜੇ (machine.acpi_rsdp) {
		ਸਥਿਤੀ = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} ਨਹੀਂਤਾਂ {
		ਸਥਿਤੀ = 0;
	}
	ਜੇ (!ਸਥਿਤੀ) {
		/* try something different like MPS */
	}

	ਵਾਪਸ ਸਥਿਤੀ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP

ਖਾਲੀ apic_send_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਪੂਰਨ type)
{
	u32_t icr1, icr2;

	ਜੇ (ncpus == 1)
		/* no need of sending an IPI */
		ਵਾਪਸ;

	ਜਦੋਂ (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	ਚੋਣ (type) {
		ਹਾਲਤ APIC_IPI_DEST:
			ਜੇ (!cpu_is_ready(cpu))
				ਵਾਪਸ;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ਖੇਤਰ | ਸਦਿਸ਼);
			ਰੋਕੋ;
		ਹਾਲਤ APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | ਸਦਿਸ਼);
			ਰੋਕੋ;
		ਹਾਲਤ APIC_IPI_ਮੰਜ਼ਿਲ_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | ਸਦਿਸ਼);
			ਰੋਕੋ;
		ਹਾਲਤ APIC_IPI_ਮੰਜ਼ਿਲ_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | ਸਦਿਸ਼);
			ਰੋਕੋ;
		ਮੂਲ:
			ਲਿਖੋ("WARNING : unknown send ipi type request\n");
	}

}

ਪੂਰਨ apic_send_startup_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, phys_ਬਾਈਟਾਂ trampoline)
{
	ਪੂਰਨ timeout;
	u32_t errstatus = 0;
	ਪੂਰਨ i;

	/* INIT-SIPI-SIPI sequence */

	ਲਈ (i = 0; i < 2; i++) {
		u32_t val;
		
		/* clear err status */
		lapic_errstatus();

		/* set target pe */
		val = lapic_read(LAPIC_ICR2) & 0xFFFFFF;
		val |= cpuid2apicid[cpu] << 24;
		lapic_write(LAPIC_ICR2, val);

		/* send SIPI */
		val = lapic_read(LAPIC_ICR1) & 0xFFF32000;
		val |= APIC_ICR_LEVEL_ASSERT |APIC_ICR_DM_STARTUP;
		val |= (((u32_t)trampoline >> 12)&0xff);
		lapic_write(LAPIC_ICR1, val);

		timeout = 1000;

		/* wait for 200 micro-seconds*/
		lapic_microsec_sleep (200);
		errstatus = 0;

		ਜਦੋਂ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			ਜੇ (!timeout) ਰੋਕੋ;
		}

		/* skip this one and continue with another cpu */
		ਜੇ (errstatus)
			ਵਾਪਸ -1;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ apic_send_init_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, phys_ਬਾਈਟਾਂ trampoline) 
{
	u32_t ptr, errstatus = 0;
	ਪੂਰਨ timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), ਆਕਾਰ(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), ਆਕਾਰ(u16_t ));

	/* set shutdown code */
	outb (RTC_ਸੂਚਕਾਂਕ, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(ਖਾਲੀ) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	ਜਦੋਂ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ਜੇ (!timeout) ਰੋਕੋ;
	}

	ਜੇ (errstatus) 
		ਵਾਪਸ -1; /* to continue with a new processor */

	/* clear error state register. */
	lapic_errstatus();

	/* deassert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DEST_ALL | APIC_ICR_TM_LEVEL);

	timeout = 1000;
	errstatus = 0;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	ਜਦੋਂ ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ਜੇ(!timeout) ਰੋਕੋ;
	}

	ਜੇ (errstatus) 
		ਵਾਪਸ -1; /* with the new processor */

	/* clear error state register.  */
	(ਖਾਲੀ) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
ਪੂਰਨ apic_single_cpu_init(ਖਾਲੀ)
{
	ਜੇ (!cpu_feature_apic_on_chip())
		ਵਾਪਸ 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	ਜੇ (!lapic_enable(0)) {
		lapic_addr = 0x0;
		ਵਾਪਸ 0;
	}

	bsp_lapic_id = apicid();
	ਲਿਖੋ("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	ਜੇ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ਵਾਪਸ 0;
	}

	ioapic_enable_all();

	ਜੇ (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	ਵਾਪਸ 1;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ eoi_ਵਿਧੀ_t ਸੈੱਟ_eoi_ਵਿਧੀ(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	ਜੇ (irq < 16)
		ਵਾਪਸ ioapic_eoi_ਕਿਨਾਰਾ;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ ioapic_eoi_level;
}

ਖਾਲੀ ਸੈੱਟ_irq_redir_ਨੀਵਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ irq, u32_t * ਨੀਵਾਂ)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_ਸਦਿਸ਼ | APIC_ICR_INT_ਨਕਾਬ |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	ਜੇ (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	ਨਹੀਂਤਾਂ {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].ਸਦਿਸ਼;

	*ਨੀਵਾਂ = val;
}

ਖਾਲੀ ioapic_ਸੈੱਟ_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ioa;

	ਪ੍ਰਤਿਜ੍ਞਾ(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	ਜੇ (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		ਵਾਪਸ;
	
	ਪ੍ਰਤਿਜ੍ਞਾ(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	ਲਈ (ioa = 0; ioa < nioapics; ioa++) {
		ਜੇ (io_apic[ioa].gsi_ਅਧਾਰ <= irq &&
				io_apic[ioa].gsi_ਅਧਾਰ +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, ਨੀਵਾਂ_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_ਅਧਾਰ;
			io_apic_irq[irq].eoi = ਸੈੱਟ_eoi_ਵਿਧੀ(irq);
			io_apic_irq[irq].ਸਦਿਸ਼ = LAPIC_ਸਦਿਸ਼(irq);

			ਸੈੱਟ_irq_redir_ਨੀਵਾਂ(irq, &ਨੀਵਾਂ_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_ਇੰਦਰਾਜ_write((ਖਾਲੀ *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, ਨੀਵਾਂ_32);
		}
	}
}

ਖਾਲੀ ioapic_unset_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ irq)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = ਨਿਰਾ;
	io_apic_irq[irq].eoi = ਨਿਰਾ;
}

ਖਾਲੀ ioapic_reset_pic(ਖਾਲੀ)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

ਸਥਿਰ ਖਾਲੀ irq_lapic_ਸਥਿਤੀ(ਪੂਰਨ irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	ਪੂਰਨ ਸਦਿਸ਼;
	ਬਣਤਰ irq * intr;

	intr = &io_apic_irq[irq];
	
	ਜੇ (!intr->ioa)
		ਵਾਪਸ;

	ਸਦਿਸ਼ = LAPIC_ਸਦਿਸ਼(irq);
	tmr =  apic_read_tmr_ਸਦਿਸ਼(ਸਦਿਸ਼);
	irr =  apic_read_irr_ਸਦਿਸ਼(ਸਦਿਸ਼);
	isr =  apic_read_isr_ਸਦਿਸ਼(ਸਦਿਸ਼);


	ਜੇ (lapic_test_delivery_val(isr, ਸਦਿਸ਼)) {
		ਲਿਖੋ("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, ਸਦਿਸ਼,
				lapic_test_delivery_val(tmr, ਸਦਿਸ਼) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, ਸਦਿਸ਼) ? 1 : 0,
				lapic_test_delivery_val(isr, ਸਦਿਸ਼) ? 1 : 0);
	} ਨਹੀਂਤਾਂ {
		ਲਿਖੋ("IRQ %d vec %d irr %d\n",
				irq, ਸਦਿਸ਼,
				lapic_test_delivery_val(irr, ਸਦਿਸ਼) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_ਸਾਰਣੀ + intr->pin * 2);
	ਲਿਖੋ("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->ਸਦਿਸ਼,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

ਖਾਲੀ dump_apic_irq_state(ਖਾਲੀ)
{
	ਪੂਰਨ irq;

	ਲਿਖੋ("--- IRQs state dump ---\n");
	ਲਈ (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_ਸਥਿਤੀ(irq);
	}
	ਲਿਖੋ("--- all ---\n");
}
