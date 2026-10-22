/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#அந்தர்பாவ <assert.h>

#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <minix/portio.h>

#அந்தர்பாவ <minix/syslib.h>
#அந்தர்பாவ <machine/cmos.h>

#அந்தர்பாவ <minix/u64.h>

#அந்தர்பாவ "apic.h"
#அந்தர்பாவ "apic_asm.h"
#அந்தர்பாவ "kernel/clock.h"
#அந்தர்பாவ "glo.h"
#அந்தர்பாவ "hw_intr.h"

#அந்தர்பாவ "acpi.h"

#ifdef USE_WATCHDOG
#அந்தர்பாவ "kernel/watchdog.h"
#endif

#define APIC_ENABLE		0x100
#define APIC_FOCUS_DISABLED	(1 << 9)
#define APIC_SIV		0xFF

#define APIC_TDCR_2	0x00
#define APIC_TDCR_4	0x01
#define APIC_TDCR_8	0x02
#define APIC_TDCR_16	0x03
#define APIC_TDCR_32	0x08
#define APIC_TDCR_64	0x09
#define APIC_TDCR_128	0x0a
#define APIC_TDCR_1	0x0b

#define IS_SET(mask)		(mask)
#define IS_CLEAR(mask)		0

#define APIC_LVTT_VECTOR_MASK	0x000000FF
#define APIC_LVTT_DS_PENDING	(1 << 12)
#define APIC_LVTT_MASK		(1 << 16)
#define APIC_LVTT_TM		(1 << 17)

#define APIC_LVT_IIPP_MASK	0x00002000
#define APIC_LVT_IIPP_AH	0x00002000
#define APIC_LVT_IIPP_AL	0x00000000

#define APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#define APIC_LVT_TM_PERIODIC	IS_SET(APIC_LVTT_TM)

#define IOAPIC_REGSEL		0x0
#define IOAPIC_RW		0x10

#define APIC_ICR_DM_MASK		0x00000700
#define APIC_ICR_VECTOR			APIC_LVTT_VECTOR_MASK
#define APIC_ICR_DM_FIXED		(0 << 8)
#define APIC_ICR_DM_LOWEST_PRIORITY	(1 << 8)
#define APIC_ICR_DM_SMI			(2 << 8)
#define APIC_ICR_DM_RESERVED		(3 << 8)
#define APIC_ICR_DM_NMI			(4 << 8)
#define APIC_ICR_DM_INIT		(5 << 8)
#define APIC_ICR_DM_STARTUP		(6 << 8)
#define APIC_ICR_DM_EXTINT		(7 << 8)

#define APIC_ICR_DM_PHYSICAL		(0 << 11)
#define APIC_ICR_DM_LOGICAL		(1 << 11)

#define APIC_ICR_DELIVERY_PENDING	(1 << 12)

#define APIC_ICR_INT_POLARITY		(1 << 13)
#define APIC_ICR_INTPOL_கீழ்		IS_SET(APIC_ICR_INT_POLARITY)
#define APIC_ICR_INTPOL_உயர்வு		IS_CLEAR(APIC_ICR_INT_POLARITY)

#define APIC_ICR_LEVEL_ASSERT		(1 << 14)
#define APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#define APIC_ICR_TRIGGER		(1 << 15)
#define APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#define APIC_ICR_TM_EDGE		IS_CLEAR(APIC_ICR_TRIGGER)

#define APIC_ICR_INT_MASK		(1 << 16)

#define APIC_ICR_DEST_FIELD		(0 << 18)
#define APIC_ICR_DEST_SELF		(1 << 18)
#define APIC_ICR_DEST_ALL		(2 << 18)
#define APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#define IA32_APIC_BASE	0x1b
#define IA32_APIC_BASE_ENABLE_BIT	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#define LAPIC_VECTOR(irq)	(IRQ0_VECTOR +(irq))

#define IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#define SPL0				0x0
#define	SPLHI				0xF


கட்டமைப்பு io_apic io_apic[MAX_NR_IOAPICS];
unsigned nioapics;

கட்டமைப்பு irq;
வகைவரை வெறுமை (* eoi_method_t)(கட்டமைப்பு irq *);

கட்டமைப்பு irq {
	கட்டமைப்பு io_apic * 	ioa;
	unsigned		pin;
	unsigned		vector;
	eoi_method_t		eoi;
	unsigned		state;
};

நிலையான கட்டமைப்பு irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#ifdef CONFIG_SMP

#அந்தர்பாவ "kernel/smp.h"

#endif

#அந்தர்பாவ "kernel/spinlock.h"


#define lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#define lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#define lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#define is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#define VERBOSE_APIC(x) x

முழு ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_bytes lapic_addr;
vir_bytes lapic_eoi_addr;
முழு bsp_lapic_id;

நிலையான நிலையற்ற unsigned probe_ticks;
நிலையான	u64_t tsc0, tsc1;
நிலையான	u32_t lapic_tctr0, lapic_tctr1;

நிலையான unsigned apic_imcrp;
நிலையான மாறிலி unsigned nlints = 0;

வெறுமை arch_eoi(வெறுமை)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
நிலையான u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#define PROBE_TICKS	(system_hz / 10)

#define IOAPIC_IOREGSEL	0x0
#define IOAPIC_IOWIN	0x10

நிலையான u32_t ioapic_read(u32_t ioa_base, u32_t reg)
{
	*((நிலையற்ற u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = (reg & 0xff);
	திரும்பு *(நிலையற்ற u32_t *)(ioa_base + IOAPIC_IOWIN);
}

நிலையான வெறுமை ioapic_write(u32_t ioa_base, u8_t reg, u32_t val)
{
	*((நிலையற்ற u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = reg;
	*((நிலையற்ற u32_t *)(ioa_base + IOAPIC_IOWIN)) = val;
}

வெறுமை lapic_microsec_sleep(unsigned மொத்தம்);
வெறுமை apic_idt_init(மாறிலி முழு reset);

நிலையான வெறுமை ioapic_enable_pin(vir_bytes ioapic_addr, முழு pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo &= ~APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

நிலையான வெறுமை ioapic_disable_pin(vir_bytes ioapic_addr, முழு pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo |= APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

#எனில் 0
நிலையான வெறுமை ioapic_redirt_entry_read(வெறுமை * ioapic_addr,
					முழு entry,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1));

}
#endif

நிலையான வெறுமை ioapic_redirt_entry_write(வெறுமை * ioapic_addr,
					முழு entry,
					u32_t hi,
					u32_t lo)
{
#எனில் 0
	VERBOSE_APIC(printf("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", entry, hi, lo));
#endif
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2), lo);
}

#define apic_read_tmr_vector(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#define apic_read_irr_vector(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#define apic_read_isr_vector(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#define lapic_test_delivery_val(val, vector) ((val) & (1 << ((vector) & 0x1f)))

நிலையான வெறுமை ioapic_eoi_level(கட்டமைப்பு irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_vector(irq->vector);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	எனில் (!lapic_test_delivery_val(tmr, irq->vector)) {
		முழு is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_MASK;

		/* set mask and edge */
		lo |= APIC_ICR_INT_MASK;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		எனில் (is_masked)
			lo |= APIC_ICR_INT_MASK;
		இல்லையெனில்
			lo &= ~APIC_ICR_INT_MASK;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2, lo);
	}
}

நிலையான வெறுமை ioapic_eoi_edge(__unused கட்டமைப்பு irq * irq)
{
	apic_eoi();
}

வெறுமை ioapic_eoi(முழு irq)
{
	எனில் (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	இல்லையெனில்
		irq_8259_eoi(irq);
}
 
வெறுமை ioapic_set_id(u32_t addr, unsigned முழு id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

முழு ioapic_enable_all(வெறுமை)
{
	i8259_disable();

	எனில் (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	திரும்பு ioapic_enabled = 1;
}

/* disables a single IO APIC */
நிலையான வெறுமை ioapic_disable(கட்டமைப்பு io_apic * ioapic)
{
	unsigned p;
	
	க்கு (p = 0; p < io_apic->pins; p++) {
		u32_t கீழ்_32, hi_32;
		கீழ்_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1));

		எனில் (!(கீழ்_32 & APIC_ICR_INT_MASK)) {
			கீழ்_32 |= APIC_ICR_INT_MASK;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2), கீழ்_32);
		}
	}
}

/* disables all IO APICs */
வெறுமை ioapic_disable_all(வெறுமை)
{
	unsigned ioa;
	எனில் (!ioapic_enabled)
		திரும்பு;

	க்கு (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	எனில் (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

நிலையான வெறுமை ioapic_disable_irq(unsigned irq)
{
	எனில்(!(io_apic_irq[irq].ioa)) {
		printf("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		திரும்பு;
	}

	ப்ரதிஜ்ஞா(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

நிலையான வெறுமை ioapic_enable_irq(unsigned irq)
{
	எனில்(!(io_apic_irq[irq].ioa)) {
		printf("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		திரும்பு;
	}

	ப்ரதிஜ்ஞா(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

வெறுமை ioapic_unmask_irq(unsigned irq)
{
	எனில் (ioapic_enabled)
		ioapic_enable_irq(irq);
	இல்லையெனில்
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

வெறுமை ioapic_mask_irq(unsigned irq)
{
	எனில் (ioapic_enabled)
		ioapic_disable_irq(irq);
	இல்லையெனில்
		/* FIXME unlikely */
		irq_8259_mask(irq);
}

unsigned முழு apicid(வெறுமை)
{
	திரும்பு lapic_read(LAPIC_ID) >> 24;
}

நிலையான முழு calib_clk_handler(irq_hook_t * UNUSED(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	எனில் (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	இல்லையெனில் எனில் (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	திரும்பு 1;
}

நிலையான முழு spurious_irq_handler(irq_hook_t * UNUSED(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	திரும்பு 1;
}

நிலையான வெறுமை apic_calibrate_clocks(unsigned cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(printf("Calibrating clock\n"));
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
	lvtt |= APIC_LVTT_MASK;
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
	போது(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_LOCK();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(printf("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_set_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (unsigned long)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

வெறுமை lapic_set_timer_one_shot(மாறிலி u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_per_us;
	மாறிலி u8_t cpu = cpuid;

	ticks_per_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_per_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);
}

வெறுமை lapic_set_timer_periodic(மாறிலி unsigned freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_per_clock_tick;
	மாறிலி u8_t cpu = cpuid;

	lapic_ticks_per_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_per_clock_tick);
}

வெறுமை lapic_stop_timer(வெறுமை)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_MASK);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

வெறுமை lapic_restart_timer(வெறுமை)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	எனில் (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_set_timer_one_shot(1000000/system_hz);
}

வெறுமை lapic_microsec_sleep(unsigned மொத்தம்)
{
	lapic_set_timer_one_shot(மொத்தம்);
	போது (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

நிலையான  u32_t lapic_errstatus(வெறுமை)
{
	lapic_write(LAPIC_ESR, 0);
	திரும்பு lapic_read(LAPIC_ESR);
}

#ifdef CONFIG_SMP
நிலையான முழு lapic_disable_in_msr(வெறுமை)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	திரும்பு 1;
}
#endif /* CONFIG_SMP */

வெறுமை lapic_disable(வெறுமை)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	எனில் (!lapic_addr)
		திரும்பு;
	
#ifdef CONFIG_SMP
	எனில் (cpu_is_bsp(cpuid) && !apic_imcrp)
#endif
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_MASK|APIC_ICR_INT_MASK);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		திரும்பு;
	}

#ifdef CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_MASK;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_MASK;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_in_msr();
#endif /* CONFIG_SMP */
}

நிலையான முழு lapic_enable_in_msr(வெறுமை)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

#எனில் 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	எனில் (addr != (lapic_addr >> 12)) {
		எனில் (msr_hi & 0xf) {
			printf("ERROR : APIC address needs more then 32 bits\n");
			திரும்பு 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#endif

	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	திரும்பு 1;
}

முழு lapic_enable(unsigned cpu)
{
	u32_t val, nlvt;

	எனில் (!lapic_addr)
		திரும்பு 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	எனில் (!cpu_has_tsc) {
		printf("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		திரும்பு 0;
	}

	எனில் (!lapic_enable_in_msr())
		திரும்பு 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_VECTOR;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(வெறுமை) lapic_read(LAPIC_SIVR);

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

	nlvt = (lapic_read(LAPIC_VERSION)>>16) & 0xFF;

	எனில்(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_MASK);
	}

	எனில்(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_MASK);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(வெறுமை) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(printf("APIC timer calibrated\n"));

	திரும்பு 1;
}

வெறுமை apic_spurios_intr_handler(வெறுமை)
{
	நிலையான unsigned x;

	x++;
	எனில் (x == 1 || (x % 100) == 0)
		printf("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

வெறுமை apic_error_intr_handler(வெறுமை)
{
	நிலையான unsigned x;

	x++;
	எனில் (x == 1 || (x % 100) == 0)
		printf("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

நிலையான கட்டமைப்பு gate_table_s gate_table_ioapic[] = {
	{ apic_hwint0, LAPIC_VECTOR( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_VECTOR( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_VECTOR( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_VECTOR( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_VECTOR( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_VECTOR( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_VECTOR( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_VECTOR( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_VECTOR( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_VECTOR( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_VECTOR(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_VECTOR(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_VECTOR(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_VECTOR(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_VECTOR(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_VECTOR(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_VECTOR(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_VECTOR(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_VECTOR(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_VECTOR(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_VECTOR(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_VECTOR(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_VECTOR(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_VECTOR(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_VECTOR(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_VECTOR(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_VECTOR(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_VECTOR(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_VECTOR(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_VECTOR(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_VECTOR(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_VECTOR(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_VECTOR(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_VECTOR(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_VECTOR(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_VECTOR(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_VECTOR(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_VECTOR(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_VECTOR(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_VECTOR(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_VECTOR(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_VECTOR(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_VECTOR(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_VECTOR(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_VECTOR(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_VECTOR(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_VECTOR(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_VECTOR(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_VECTOR(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_VECTOR(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_VECTOR(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_VECTOR(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_VECTOR(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_VECTOR(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_VECTOR(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_VECTOR(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_VECTOR(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_VECTOR(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_VECTOR(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_VECTOR(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_VECTOR(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_VECTOR(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_VECTOR(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_VECTOR(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_VECTOR, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_VECTOR, INTR_PRIVILEGE },
	{ வெற்று, 0, 0}
};

நிலையான கட்டமைப்பு gate_table_s gate_table_common[] = {
	{ ipc_entry_softint_orig, IPC_VECTOR_ORIG, USER_PRIVILEGE },
	{ kernel_call_entry_orig, KERN_CALL_VECTOR_ORIG, USER_PRIVILEGE },
	{ ipc_entry_softint_um, IPC_VECTOR_UM, USER_PRIVILEGE },
	{ kernel_call_entry_um, KERN_CALL_VECTOR_UM, USER_PRIVILEGE },
	{ வெற்று, 0, 0}
};

#ifdef CONFIG_SMP
நிலையான கட்டமைப்பு gate_table_s gate_table_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_VECTOR, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_VECTOR, INTR_PRIVILEGE },
	{ வெற்று, 0, 0}
};
#endif

#ifdef APIC_DEBUG
நிலையான வெறுமை lapic_set_dummy_handlers(வெறுமை)
{
	எழுத்து * handler;
	முழு vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_start;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_SIZE;
	க்கு(; handler < &lapic_intr_dummy_handles_end;
			handler += LAPIC_INTR_DUMMY_HANDLER_SIZE) {
		int_gate_idt(vect++, (vir_bytes) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_SHIFT));
	}
}
#endif

/* Build descriptors for interrupt gates in IDT. */
வெறுமை apic_idt_init(மாறிலி முழு reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	முழு is_bsp;

	எனில் (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_table_common);
		திரும்பு;
	}

	is_bsp = is_boot_apic(apicid());

#ifdef APIC_DEBUG
	எனில் (is_bsp)
		printf("APIC debugging is enabled\n");
	lapic_set_dummy_handlers();
#endif

	/* Build descriptors for interrupt gates in IDT. */
	எனில் (ioapic_enabled)
		idt_copy_vectors(gate_table_ioapic);
	இல்லையெனில்
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_table_common);

#ifdef CONFIG_SMP
	idt_copy_vectors(gate_table_smp);
#endif

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_VECTOR;
	val &= ~ APIC_ICR_INT_MASK;
	lapic_write(LAPIC_LVTER, val);
	(வெறுமை) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	எனில் (is_bsp) {
		BOOT_VERBOSE(printf("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_VECTOR, (vir_bytes) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_SHIFT));
	}

}

நிலையான முழு acpi_get_ioapics(கட்டமைப்பு io_apic * ioa, unsigned * nioa, unsigned max)
{
	unsigned n = 0;
	கட்டமைப்பு acpi_madt_ioapic * acpi_ioa;

	போது (n < max) {
		acpi_ioa = acpi_get_ioapic_next();
		எனில் (acpi_ioa == வெற்று)
			நிறுத்து;

		ப்ரதிஜ்ஞா(acpi_ioa->address);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->address;
		ioa[n].paddr = (phys_bytes) acpi_ioa->address;
		ioa[n].gsi_base = acpi_ioa->global_int_base;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_VERSION) & 0xff0000) >> 16)+1;
		printf("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	திரும்பு n;
}

முழு detect_ioapics(வெறுமை)
{
	முழு status;

	எனில் (machine.acpi_rsdp) {
		status = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} இல்லையெனில் {
		status = 0;
	}
	எனில் (!status) {
		/* try something different like MPS */
	}

	திரும்பு status;
}

#ifdef CONFIG_SMP

வெறுமை apic_send_ipi(unsigned vector, unsigned cpu, முழு type)
{
	u32_t icr1, icr2;

	எனில் (ncpus == 1)
		/* no need of sending an IPI */
		திரும்பு;

	போது (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	தேர்வு (type) {
		நிலை APIC_IPI_DEST:
			எனில் (!cpu_is_ready(cpu))
				திரும்பு;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_FIELD | vector);
			நிறுத்து;
		நிலை APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | vector);
			நிறுத்து;
		நிலை APIC_IPI_TO_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | vector);
			நிறுத்து;
		நிலை APIC_IPI_TO_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | vector);
			நிறுத்து;
		இயல்பு:
			printf("WARNING : unknown send ipi type request\n");
	}

}

முழு apic_send_startup_ipi(unsigned cpu, phys_bytes trampoline)
{
	முழு timeout;
	u32_t errstatus = 0;
	முழு i;

	/* INIT-SIPI-SIPI sequence */

	க்கு (i = 0; i < 2; i++) {
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

		போது ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			எனில் (!timeout) நிறுத்து;
		}

		/* skip this one and continue with another cpu */
		எனில் (errstatus)
			திரும்பு -1;
	}

	திரும்பு 0;
}

முழு apic_send_init_ipi(unsigned cpu, phys_bytes trampoline) 
{
	u32_t ptr, errstatus = 0;
	முழு timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), அளவு(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), அளவு(u16_t ));

	/* set shutdown code */
	outb (RTC_INDEX, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(வெறுமை) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	போது ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		எனில் (!timeout) நிறுத்து;
	}

	எனில் (errstatus) 
		திரும்பு -1; /* to continue with a new processor */

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

	போது ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		எனில்(!timeout) நிறுத்து;
	}

	எனில் (errstatus) 
		திரும்பு -1; /* with the new processor */

	/* clear error state register.  */
	(வெறுமை) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	திரும்பு 0;
}
#endif

#ifndef CONFIG_SMP
முழு apic_single_cpu_init(வெறுமை)
{
	எனில் (!cpu_feature_apic_on_chip())
		திரும்பு 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	எனில் (!lapic_enable(0)) {
		lapic_addr = 0x0;
		திரும்பு 0;
	}

	bsp_lapic_id = apicid();
	printf("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	எனில் (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		திரும்பு 0;
	}

	ioapic_enable_all();

	எனில் (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	திரும்பு 1;
}
#endif

நிலையான eoi_method_t set_eoi_method(unsigned irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	எனில் (irq < 16)
		திரும்பு ioapic_eoi_edge;
	இல்லையெனில்
		திரும்பு ioapic_eoi_level;
}

வெறுமை set_irq_redir_கீழ்(unsigned irq, u32_t * கீழ்)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_VECTOR | APIC_ICR_INT_MASK |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	எனில் (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	இல்லையெனில் {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].vector;

	*கீழ் = val;
}

வெறுமை ioapic_set_irq(unsigned irq)
{
	unsigned ioa;

	ப்ரதிஜ்ஞா(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	எனில் (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		திரும்பு;
	
	ப்ரதிஜ்ஞா(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	க்கு (ioa = 0; ioa < nioapics; ioa++) {
		எனில் (io_apic[ioa].gsi_base <= irq &&
				io_apic[ioa].gsi_base +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, கீழ்_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_base;
			io_apic_irq[irq].eoi = set_eoi_method(irq);
			io_apic_irq[irq].vector = LAPIC_VECTOR(irq);

			set_irq_redir_கீழ்(irq, &கீழ்_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_entry_write((வெறுமை *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, கீழ்_32);
		}
	}
}

வெறுமை ioapic_unset_irq(unsigned irq)
{
	ப்ரதிஜ்ஞா(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = வெற்று;
	io_apic_irq[irq].eoi = வெற்று;
}

வெறுமை ioapic_reset_pic(வெறுமை)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

நிலையான வெறுமை irq_lapic_status(முழு irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	முழு vector;
	கட்டமைப்பு irq * intr;

	intr = &io_apic_irq[irq];
	
	எனில் (!intr->ioa)
		திரும்பு;

	vector = LAPIC_VECTOR(irq);
	tmr =  apic_read_tmr_vector(vector);
	irr =  apic_read_irr_vector(vector);
	isr =  apic_read_isr_vector(vector);


	எனில் (lapic_test_delivery_val(isr, vector)) {
		printf("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, vector,
				lapic_test_delivery_val(tmr, vector) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, vector) ? 1 : 0,
				lapic_test_delivery_val(isr, vector) ? 1 : 0);
	} இல்லையெனில் {
		printf("IRQ %d vec %d irr %d\n",
				irq, vector,
				lapic_test_delivery_val(irr, vector) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_TABLE + intr->pin * 2);
	printf("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->vector,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

வெறுமை dump_apic_irq_state(வெறுமை)
{
	முழு irq;

	printf("--- IRQs state dump ---\n");
	க்கு (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_status(irq);
	}
	printf("--- all ---\n");
}
