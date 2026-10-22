/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <minix/portio.h>

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <machine/cmos.h>

#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "apic.h"
#ಅನ್ತರ್ಭಾವ "apic_asm.h"
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ "glo.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"

#ಅನ್ತರ್ಭಾವ "acpi.h"

#ifdef USE_WATCHDOG
#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"
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
#define APIC_ICR_INTPOL_ಕೆಳಗಿನ		IS_SET(APIC_ICR_INT_POLARITY)
#define APIC_ICR_INTPOL_ಮೇಲಿನ		IS_CLEAR(APIC_ICR_INT_POLARITY)

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


ಸಂರಚನಾ io_apic io_apic[MAX_NR_IOAPICS];
unsigned nioapics;

ಸಂರಚನಾ irq;
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (* eoi_method_t)(ಸಂರಚನಾ irq *);

ಸಂರಚನಾ irq {
	ಸಂರಚನಾ io_apic * 	ioa;
	unsigned		pin;
	unsigned		vector;
	eoi_method_t		eoi;
	unsigned		state;
};

ಸ್ಥಿರ ಸಂರಚನಾ irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#ifdef CONFIG_SMP

#ಅನ್ತರ್ಭಾವ "kernel/smp.h"

#endif

#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"


#define lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#define lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#define lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#define is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#define VERBOSE_APIC(x) x

ಪೂರ್ಣಾಙ್ಕ ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_bytes lapic_addr;
vir_bytes lapic_eoi_addr;
ಪೂರ್ಣಾಙ್ಕ bsp_lapic_id;

ಸ್ಥಿರ ಚಞ್ಚಲ unsigned probe_ticks;
ಸ್ಥಿರ	u64_t tsc0, tsc1;
ಸ್ಥಿರ	u32_t lapic_tctr0, lapic_tctr1;

ಸ್ಥಿರ unsigned apic_imcrp;
ಸ್ಥಿರ ಧ್ರುವ unsigned nlints = 0;

ಶೂನ್ಯ arch_eoi(ಶೂನ್ಯ)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
ಸ್ಥಿರ u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#define PROBE_TICKS	(system_hz / 10)

#define IOAPIC_IOREGSEL	0x0
#define IOAPIC_IOWIN	0x10

ಸ್ಥಿರ u32_t ioapic_read(u32_t ioa_base, u32_t reg)
{
	*((ಚಞ್ಚಲ u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = (reg & 0xff);
	ಪ್ರತ್ಯಾಗಮ *(ಚಞ್ಚಲ u32_t *)(ioa_base + IOAPIC_IOWIN);
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_write(u32_t ioa_base, u8_t reg, u32_t val)
{
	*((ಚಞ್ಚಲ u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = reg;
	*((ಚಞ್ಚಲ u32_t *)(ioa_base + IOAPIC_IOWIN)) = val;
}

ಶೂನ್ಯ lapic_microsec_sleep(unsigned ಎಣಿಕೆ);
ಶೂನ್ಯ apic_idt_init(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ reset);

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_enable_pin(vir_bytes ioapic_addr, ಪೂರ್ಣಾಙ್ಕ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo &= ~APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable_pin(vir_bytes ioapic_addr, ಪೂರ್ಣಾಙ್ಕ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo |= APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

#ಯದಿ 0
ಸ್ಥಿರ ಶೂನ್ಯ ioapic_redirt_entry_read(ಶೂನ್ಯ * ioapic_addr,
					ಪೂರ್ಣಾಙ್ಕ entry,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1));

}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_redirt_entry_write(ಶೂನ್ಯ * ioapic_addr,
					ಪೂರ್ಣಾಙ್ಕ entry,
					u32_t hi,
					u32_t lo)
{
#ಯದಿ 0
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

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_eoi_level(ಸಂರಚನಾ irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_vector(irq->vector);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	ಯದಿ (!lapic_test_delivery_val(tmr, irq->vector)) {
		ಪೂರ್ಣಾಙ್ಕ is_masked;
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
		ಯದಿ (is_masked)
			lo |= APIC_ICR_INT_MASK;
		ಅನ್ಯಥಾ
			lo &= ~APIC_ICR_INT_MASK;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2, lo);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_eoi_edge(__unused ಸಂರಚನಾ irq * irq)
{
	apic_eoi();
}

ಶೂನ್ಯ ioapic_eoi(ಪೂರ್ಣಾಙ್ಕ irq)
{
	ಯದಿ (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	ಅನ್ಯಥಾ
		irq_8259_eoi(irq);
}
 
ಶೂನ್ಯ ioapic_set_id(u32_t addr, unsigned ಪೂರ್ಣಾಙ್ಕ id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

ಪೂರ್ಣಾಙ್ಕ ioapic_enable_all(ಶೂನ್ಯ)
{
	i8259_disable();

	ಯದಿ (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	ಪ್ರತ್ಯಾಗಮ ioapic_enabled = 1;
}

/* disables a single IO APIC */
ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable(ಸಂರಚನಾ io_apic * ioapic)
{
	unsigned p;
	
	ಪ್ರತಿ (p = 0; p < io_apic->pins; p++) {
		u32_t ಕೆಳಗಿನ_32, hi_32;
		ಕೆಳಗಿನ_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1));

		ಯದಿ (!(ಕೆಳಗಿನ_32 & APIC_ICR_INT_MASK)) {
			ಕೆಳಗಿನ_32 |= APIC_ICR_INT_MASK;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2), ಕೆಳಗಿನ_32);
		}
	}
}

/* disables all IO APICs */
ಶೂನ್ಯ ioapic_disable_all(ಶೂನ್ಯ)
{
	unsigned ioa;
	ಯದಿ (!ioapic_enabled)
		ಪ್ರತ್ಯಾಗಮ;

	ಪ್ರತಿ (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	ಯದಿ (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable_irq(unsigned irq)
{
	ಯದಿ(!(io_apic_irq[irq].ioa)) {
		printf("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿಜ್ಞಾ(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_enable_irq(unsigned irq)
{
	ಯದಿ(!(io_apic_irq[irq].ioa)) {
		printf("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿಜ್ಞಾ(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

ಶೂನ್ಯ ioapic_unmask_irq(unsigned irq)
{
	ಯದಿ (ioapic_enabled)
		ioapic_enable_irq(irq);
	ಅನ್ಯಥಾ
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

ಶೂನ್ಯ ioapic_mask_irq(unsigned irq)
{
	ಯದಿ (ioapic_enabled)
		ioapic_disable_irq(irq);
	ಅನ್ಯಥಾ
		/* FIXME unlikely */
		irq_8259_mask(irq);
}

unsigned ಪೂರ್ಣಾಙ್ಕ apicid(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ lapic_read(LAPIC_ID) >> 24;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ calib_clk_handler(irq_hook_t * UNUSED(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	ಯದಿ (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	ಅನ್ಯಥಾ ಯದಿ (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spurious_irq_handler(irq_hook_t * UNUSED(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ apic_calibrate_clocks(unsigned cpu)
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
	ಯಾವತ್(probe_ticks < PROBE_TICKS) {
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

ಶೂನ್ಯ lapic_set_timer_one_shot(ಧ್ರುವ u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_per_us;
	ಧ್ರುವ u8_t cpu = cpuid;

	ticks_per_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_per_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);
}

ಶೂನ್ಯ lapic_set_timer_periodic(ಧ್ರುವ unsigned freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_per_clock_tick;
	ಧ್ರುವ u8_t cpu = cpuid;

	lapic_ticks_per_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_per_clock_tick);
}

ಶೂನ್ಯ lapic_stop_timer(ಶೂನ್ಯ)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_MASK);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

ಶೂನ್ಯ lapic_restart_timer(ಶೂನ್ಯ)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	ಯದಿ (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_set_timer_one_shot(1000000/system_hz);
}

ಶೂನ್ಯ lapic_microsec_sleep(unsigned ಎಣಿಕೆ)
{
	lapic_set_timer_one_shot(ಎಣಿಕೆ);
	ಯಾವತ್ (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

ಸ್ಥಿರ  u32_t lapic_errstatus(ಶೂನ್ಯ)
{
	lapic_write(LAPIC_ESR, 0);
	ಪ್ರತ್ಯಾಗಮ lapic_read(LAPIC_ESR);
}

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lapic_disable_in_msr(ಶೂನ್ಯ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	ಪ್ರತ್ಯಾಗಮ 1;
}
#endif /* CONFIG_SMP */

ಶೂನ್ಯ lapic_disable(ಶೂನ್ಯ)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	ಯದಿ (!lapic_addr)
		ಪ್ರತ್ಯಾಗಮ;
	
#ifdef CONFIG_SMP
	ಯದಿ (cpu_is_bsp(cpuid) && !apic_imcrp)
#endif
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_MASK|APIC_ICR_INT_MASK);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		ಪ್ರತ್ಯಾಗಮ;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lapic_enable_in_msr(ಶೂನ್ಯ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

#ಯದಿ 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	ಯದಿ (addr != (lapic_addr >> 12)) {
		ಯದಿ (msr_hi & 0xf) {
			printf("ERROR : APIC address needs more then 32 bits\n");
			ಪ್ರತ್ಯಾಗಮ 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#endif

	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ lapic_enable(unsigned cpu)
{
	u32_t val, nlvt;

	ಯದಿ (!lapic_addr)
		ಪ್ರತ್ಯಾಗಮ 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	ಯದಿ (!cpu_has_tsc) {
		printf("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ (!lapic_enable_in_msr())
		ಪ್ರತ್ಯಾಗಮ 0;

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
	(ಶೂನ್ಯ) lapic_read(LAPIC_SIVR);

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

	ಯದಿ(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_MASK);
	}

	ಯದಿ(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_MASK);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(ಶೂನ್ಯ) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(printf("APIC timer calibrated\n"));

	ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ apic_spurios_intr_handler(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ unsigned x;

	x++;
	ಯದಿ (x == 1 || (x % 100) == 0)
		printf("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

ಶೂನ್ಯ apic_error_intr_handler(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ unsigned x;

	x++;
	ಯದಿ (x == 1 || (x % 100) == 0)
		printf("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

ಸ್ಥಿರ ಸಂರಚನಾ gate_table_s gate_table_ioapic[] = {
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
	{ ರಿಕ್ತ, 0, 0}
};

ಸ್ಥಿರ ಸಂರಚನಾ gate_table_s gate_table_common[] = {
	{ ipc_entry_softint_orig, IPC_VECTOR_ORIG, USER_PRIVILEGE },
	{ kernel_call_entry_orig, KERN_CALL_VECTOR_ORIG, USER_PRIVILEGE },
	{ ipc_entry_softint_um, IPC_VECTOR_UM, USER_PRIVILEGE },
	{ kernel_call_entry_um, KERN_CALL_VECTOR_UM, USER_PRIVILEGE },
	{ ರಿಕ್ತ, 0, 0}
};

#ifdef CONFIG_SMP
ಸ್ಥಿರ ಸಂರಚನಾ gate_table_s gate_table_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_VECTOR, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_VECTOR, INTR_PRIVILEGE },
	{ ರಿಕ್ತ, 0, 0}
};
#endif

#ifdef APIC_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ lapic_set_dummy_handlers(ಶೂನ್ಯ)
{
	ವರ್ಣ * handler;
	ಪೂರ್ಣಾಙ್ಕ vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_start;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_SIZE;
	ಪ್ರತಿ(; handler < &lapic_intr_dummy_handles_end;
			handler += LAPIC_INTR_DUMMY_HANDLER_SIZE) {
		int_gate_idt(vect++, (vir_bytes) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_SHIFT));
	}
}
#endif

/* Build descriptors for interrupt gates in IDT. */
ಶೂನ್ಯ apic_idt_init(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	ಪೂರ್ಣಾಙ್ಕ is_bsp;

	ಯದಿ (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_table_common);
		ಪ್ರತ್ಯಾಗಮ;
	}

	is_bsp = is_boot_apic(apicid());

#ifdef APIC_DEBUG
	ಯದಿ (is_bsp)
		printf("APIC debugging is enabled\n");
	lapic_set_dummy_handlers();
#endif

	/* Build descriptors for interrupt gates in IDT. */
	ಯದಿ (ioapic_enabled)
		idt_copy_vectors(gate_table_ioapic);
	ಅನ್ಯಥಾ
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
	(ಶೂನ್ಯ) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	ಯದಿ (is_bsp) {
		BOOT_VERBOSE(printf("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_VECTOR, (vir_bytes) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_SHIFT));
	}

}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_get_ioapics(ಸಂರಚನಾ io_apic * ioa, unsigned * nioa, unsigned max)
{
	unsigned n = 0;
	ಸಂರಚನಾ acpi_madt_ioapic * acpi_ioa;

	ಯಾವತ್ (n < max) {
		acpi_ioa = acpi_get_ioapic_next();
		ಯದಿ (acpi_ioa == ರಿಕ್ತ)
			ಭಙ್ಗ;

		ಪ್ರತಿಜ್ಞಾ(acpi_ioa->address);

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
	ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ detect_ioapics(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ status;

	ಯದಿ (machine.acpi_rsdp) {
		status = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} ಅನ್ಯಥಾ {
		status = 0;
	}
	ಯದಿ (!status) {
		/* try something different like MPS */
	}

	ಪ್ರತ್ಯಾಗಮ status;
}

#ifdef CONFIG_SMP

ಶೂನ್ಯ apic_send_ipi(unsigned vector, unsigned cpu, ಪೂರ್ಣಾಙ್ಕ type)
{
	u32_t icr1, icr2;

	ಯದಿ (ncpus == 1)
		/* no need of sending an IPI */
		ಪ್ರತ್ಯಾಗಮ;

	ಯಾವತ್ (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	ವಿಕಲ್ಪ (type) {
		ಪಕ್ಷ APIC_IPI_DEST:
			ಯದಿ (!cpu_is_ready(cpu))
				ಪ್ರತ್ಯಾಗಮ;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_FIELD | vector);
			ಭಙ್ಗ;
		ಪಕ್ಷ APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | vector);
			ಭಙ್ಗ;
		ಪಕ್ಷ APIC_IPI_TO_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | vector);
			ಭಙ್ಗ;
		ಪಕ್ಷ APIC_IPI_TO_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | vector);
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			printf("WARNING : unknown send ipi type request\n");
	}

}

ಪೂರ್ಣಾಙ್ಕ apic_send_startup_ipi(unsigned cpu, phys_bytes trampoline)
{
	ಪೂರ್ಣಾಙ್ಕ timeout;
	u32_t errstatus = 0;
	ಪೂರ್ಣಾಙ್ಕ i;

	/* INIT-SIPI-SIPI sequence */

	ಪ್ರತಿ (i = 0; i < 2; i++) {
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

		ಯಾವತ್ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			ಯದಿ (!timeout) ಭಙ್ಗ;
		}

		/* skip this one and continue with another cpu */
		ಯದಿ (errstatus)
			ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ apic_send_init_ipi(unsigned cpu, phys_bytes trampoline) 
{
	u32_t ptr, errstatus = 0;
	ಪೂರ್ಣಾಙ್ಕ timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), ಪರಿಮಾಣ(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), ಪರಿಮಾಣ(u16_t ));

	/* set shutdown code */
	outb (RTC_INDEX, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(ಶೂನ್ಯ) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	ಯಾವತ್ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ಯದಿ (!timeout) ಭಙ್ಗ;
	}

	ಯದಿ (errstatus) 
		ಪ್ರತ್ಯಾಗಮ -1; /* to continue with a new processor */

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

	ಯಾವತ್ ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ಯದಿ(!timeout) ಭಙ್ಗ;
	}

	ಯದಿ (errstatus) 
		ಪ್ರತ್ಯಾಗಮ -1; /* with the new processor */

	/* clear error state register.  */
	(ಶೂನ್ಯ) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

#ifndef CONFIG_SMP
ಪೂರ್ಣಾಙ್ಕ apic_single_cpu_init(ಶೂನ್ಯ)
{
	ಯದಿ (!cpu_feature_apic_on_chip())
		ಪ್ರತ್ಯಾಗಮ 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	ಯದಿ (!lapic_enable(0)) {
		lapic_addr = 0x0;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	bsp_lapic_id = apicid();
	printf("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	ಯದಿ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ioapic_enable_all();

	ಯದಿ (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	ಪ್ರತ್ಯಾಗಮ 1;
}
#endif

ಸ್ಥಿರ eoi_method_t set_eoi_method(unsigned irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	ಯದಿ (irq < 16)
		ಪ್ರತ್ಯಾಗಮ ioapic_eoi_edge;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ ioapic_eoi_level;
}

ಶೂನ್ಯ set_irq_redir_ಕೆಳಗಿನ(unsigned irq, u32_t * ಕೆಳಗಿನ)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_VECTOR | APIC_ICR_INT_MASK |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	ಯದಿ (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	ಅನ್ಯಥಾ {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].vector;

	*ಕೆಳಗಿನ = val;
}

ಶೂನ್ಯ ioapic_set_irq(unsigned irq)
{
	unsigned ioa;

	ಪ್ರತಿಜ್ಞಾ(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	ಯದಿ (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		ಪ್ರತ್ಯಾಗಮ;
	
	ಪ್ರತಿಜ್ಞಾ(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	ಪ್ರತಿ (ioa = 0; ioa < nioapics; ioa++) {
		ಯದಿ (io_apic[ioa].gsi_base <= irq &&
				io_apic[ioa].gsi_base +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, ಕೆಳಗಿನ_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_base;
			io_apic_irq[irq].eoi = set_eoi_method(irq);
			io_apic_irq[irq].vector = LAPIC_VECTOR(irq);

			set_irq_redir_ಕೆಳಗಿನ(irq, &ಕೆಳಗಿನ_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_entry_write((ಶೂನ್ಯ *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, ಕೆಳಗಿನ_32);
		}
	}
}

ಶೂನ್ಯ ioapic_unset_irq(unsigned irq)
{
	ಪ್ರತಿಜ್ಞಾ(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = ರಿಕ್ತ;
	io_apic_irq[irq].eoi = ರಿಕ್ತ;
}

ಶೂನ್ಯ ioapic_reset_pic(ಶೂನ್ಯ)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

ಸ್ಥಿರ ಶೂನ್ಯ irq_lapic_status(ಪೂರ್ಣಾಙ್ಕ irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	ಪೂರ್ಣಾಙ್ಕ vector;
	ಸಂರಚನಾ irq * intr;

	intr = &io_apic_irq[irq];
	
	ಯದಿ (!intr->ioa)
		ಪ್ರತ್ಯಾಗಮ;

	vector = LAPIC_VECTOR(irq);
	tmr =  apic_read_tmr_vector(vector);
	irr =  apic_read_irr_vector(vector);
	isr =  apic_read_isr_vector(vector);


	ಯದಿ (lapic_test_delivery_val(isr, vector)) {
		printf("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, vector,
				lapic_test_delivery_val(tmr, vector) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, vector) ? 1 : 0,
				lapic_test_delivery_val(isr, vector) ? 1 : 0);
	} ಅನ್ಯಥಾ {
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

ಶೂನ್ಯ dump_apic_irq_state(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ irq;

	printf("--- IRQs state dump ---\n");
	ಪ್ರತಿ (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_status(irq);
	}
	printf("--- all ---\n");
}
