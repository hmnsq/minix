/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#समावेश <assert.h>

#समावेश <unistd.h>
#समावेश <minix/portio.h>

#समावेश <minix/syslib.h>
#समावेश <machine/cmos.h>

#समावेश <minix/u64.h>

#समावेश "apic.h"
#समावेश "apic_asm.h"
#समावेश "kernel/clock.h"
#समावेश "glo.h"
#समावेश "hw_intr.h"

#समावेश "acpi.h"

#अगर_घोषित USE_WATCHDOG
#समावेश "kernel/watchdog.h"
#पूर्ण_अगर

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

#घोषणा IS_SET(mask)		(mask)
#घोषणा IS_CLEAR(mask)		0

#घोषणा APIC_LVTT_VECTOR_MASK	0x000000FF
#घोषणा APIC_LVTT_DS_PENDING	(1 << 12)
#घोषणा APIC_LVTT_MASK		(1 << 16)
#घोषणा APIC_LVTT_TM		(1 << 17)

#घोषणा APIC_LVT_IIPP_MASK	0x00002000
#घोषणा APIC_LVT_IIPP_AH	0x00002000
#घोषणा APIC_LVT_IIPP_AL	0x00000000

#घोषणा APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#घोषणा APIC_LVT_TM_PERIODIC	IS_SET(APIC_LVTT_TM)

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
#घोषणा APIC_ICR_INTPOL_निचला		IS_SET(APIC_ICR_INT_POLARITY)
#घोषणा APIC_ICR_INTPOL_ऊपरी		IS_CLEAR(APIC_ICR_INT_POLARITY)

#घोषणा APIC_ICR_LEVEL_ASSERT		(1 << 14)
#घोषणा APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#घोषणा APIC_ICR_TRIGGER		(1 << 15)
#घोषणा APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#घोषणा APIC_ICR_TM_EDGE		IS_CLEAR(APIC_ICR_TRIGGER)

#घोषणा APIC_ICR_INT_MASK		(1 << 16)

#घोषणा APIC_ICR_DEST_FIELD		(0 << 18)
#घोषणा APIC_ICR_DEST_SELF		(1 << 18)
#घोषणा APIC_ICR_DEST_ALL		(2 << 18)
#घोषणा APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#घोषणा IA32_APIC_BASE	0x1b
#घोषणा IA32_APIC_BASE_ENABLE_BIT	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#घोषणा LAPIC_VECTOR(irq)	(IRQ0_VECTOR +(irq))

#घोषणा IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#घोषणा SPL0				0x0
#घोषणा	SPLHI				0xF


काष्ठा io_apic io_apic[MAX_NR_IOAPICS];
अचिन्हित nioapics;

काष्ठा irq;
प्रकार व्योम (* eoi_method_t)(काष्ठा irq *);

काष्ठा irq {
	काष्ठा io_apic * 	ioa;
	अचिन्हित		pin;
	अचिन्हित		vector;
	eoi_method_t		eoi;
	अचिन्हित		state;
};

अटल काष्ठा irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#अगर_घोषित CONFIG_SMP

#समावेश "kernel/smp.h"

#पूर्ण_अगर

#समावेश "kernel/spinlock.h"


#घोषणा lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#घोषणा lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#घोषणा lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#घोषणा lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#घोषणा is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#घोषणा VERBOSE_APIC(x) x

पूर्णांक ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_bytes lapic_addr;
vir_bytes lapic_eoi_addr;
पूर्णांक bsp_lapic_id;

अटल अस्थिर अचिन्हित probe_ticks;
अटल	u64_t tsc0, tsc1;
अटल	u32_t lapic_tctr0, lapic_tctr1;

अटल अचिन्हित apic_imcrp;
अटल स्थिर अचिन्हित nlints = 0;

व्योम arch_eoi(व्योम)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
अटल u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#घोषणा PROBE_TICKS	(system_hz / 10)

#घोषणा IOAPIC_IOREGSEL	0x0
#घोषणा IOAPIC_IOWIN	0x10

अटल u32_t ioapic_read(u32_t ioa_base, u32_t reg)
{
	*((अस्थिर u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = (reg & 0xff);
	वापस *(अस्थिर u32_t *)(ioa_base + IOAPIC_IOWIN);
}

अटल व्योम ioapic_write(u32_t ioa_base, u8_t reg, u32_t val)
{
	*((अस्थिर u32_t *)(ioa_base + IOAPIC_IOREGSEL)) = reg;
	*((अस्थिर u32_t *)(ioa_base + IOAPIC_IOWIN)) = val;
}

व्योम lapic_microsec_sleep(अचिन्हित गिनती);
व्योम apic_idt_init(स्थिर पूर्णांक reset);

अटल व्योम ioapic_enable_pin(vir_bytes ioapic_addr, पूर्णांक pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo &= ~APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

अटल व्योम ioapic_disable_pin(vir_bytes ioapic_addr, पूर्णांक pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);

	lo |= APIC_ICR_INT_MASK;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo);
}

#अगर 0
अटल व्योम ioapic_redirt_entry_read(व्योम * ioapic_addr,
					पूर्णांक entry,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1));

}
#पूर्ण_अगर

अटल व्योम ioapic_redirt_entry_write(व्योम * ioapic_addr,
					पूर्णांक entry,
					u32_t hi,
					u32_t lo)
{
#अगर 0
	VERBOSE_APIC(म_लिखो("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", entry, hi, lo));
#पूर्ण_अगर
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_TABLE + entry * 2), lo);
}

#घोषणा apic_read_tmr_vector(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#घोषणा apic_read_irr_vector(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#घोषणा apic_read_isr_vector(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#घोषणा lapic_test_delivery_val(val, vector) ((val) & (1 << ((vector) & 0x1f)))

अटल व्योम ioapic_eoi_level(काष्ठा irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_vector(irq->vector);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	अगर (!lapic_test_delivery_val(tmr, irq->vector)) {
		पूर्णांक is_masked;
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
		अगर (is_masked)
			lo |= APIC_ICR_INT_MASK;
		अन्यथा
			lo &= ~APIC_ICR_INT_MASK;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_TABLE + irq->pin * 2, lo);
	}
}

अटल व्योम ioapic_eoi_edge(__unused काष्ठा irq * irq)
{
	apic_eoi();
}

व्योम ioapic_eoi(पूर्णांक irq)
{
	अगर (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	अन्यथा
		irq_8259_eoi(irq);
}
 
व्योम ioapic_set_id(u32_t addr, अचिन्हित पूर्णांक id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

पूर्णांक ioapic_enable_all(व्योम)
{
	i8259_disable();

	अगर (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	वापस ioapic_enabled = 1;
}

/* disables a single IO APIC */
अटल व्योम ioapic_disable(काष्ठा io_apic * ioapic)
{
	अचिन्हित p;
	
	क्रम (p = 0; p < io_apic->pins; p++) {
		u32_t निचला_32, hi_32;
		निचला_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1));

		अगर (!(निचला_32 & APIC_ICR_INT_MASK)) {
			निचला_32 |= APIC_ICR_INT_MASK;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_TABLE + p * 2), निचला_32);
		}
	}
}

/* disables all IO APICs */
व्योम ioapic_disable_all(व्योम)
{
	अचिन्हित ioa;
	अगर (!ioapic_enabled)
		वापस;

	क्रम (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	अगर (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

अटल व्योम ioapic_disable_irq(अचिन्हित irq)
{
	अगर(!(io_apic_irq[irq].ioa)) {
		म_लिखो("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		वापस;
	}

	निश्चित(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

अटल व्योम ioapic_enable_irq(अचिन्हित irq)
{
	अगर(!(io_apic_irq[irq].ioa)) {
		म_लिखो("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		वापस;
	}

	निश्चित(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

व्योम ioapic_unmask_irq(अचिन्हित irq)
{
	अगर (ioapic_enabled)
		ioapic_enable_irq(irq);
	अन्यथा
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

व्योम ioapic_mask_irq(अचिन्हित irq)
{
	अगर (ioapic_enabled)
		ioapic_disable_irq(irq);
	अन्यथा
		/* FIXME unlikely */
		irq_8259_mask(irq);
}

अचिन्हित पूर्णांक apicid(व्योम)
{
	वापस lapic_read(LAPIC_ID) >> 24;
}

अटल पूर्णांक calib_clk_handler(irq_hook_t * UNUSED(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	अगर (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	अन्यथा अगर (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	वापस 1;
}

अटल पूर्णांक spurious_irq_handler(irq_hook_t * UNUSED(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	वापस 1;
}

अटल व्योम apic_calibrate_clocks(अचिन्हित cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(म_लिखो("Calibrating clock\n"));
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
	जबतक(probe_ticks < PROBE_TICKS) {
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
	BOOT_VERBOSE(म_लिखो("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_set_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (अचिन्हित दीर्घ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

व्योम lapic_set_timer_one_shot(स्थिर u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_per_us;
	स्थिर u8_t cpu = cpuid;

	ticks_per_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_per_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);
}

व्योम lapic_set_timer_periodic(स्थिर अचिन्हित freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_per_clock_tick;
	स्थिर u8_t cpu = cpuid;

	lapic_ticks_per_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_per_clock_tick);
}

व्योम lapic_stop_timer(व्योम)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_MASK);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

व्योम lapic_restart_timer(व्योम)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	अगर (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_set_timer_one_shot(1000000/system_hz);
}

व्योम lapic_microsec_sleep(अचिन्हित गिनती)
{
	lapic_set_timer_one_shot(गिनती);
	जबतक (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

अटल  u32_t lapic_errstatus(व्योम)
{
	lapic_write(LAPIC_ESR, 0);
	वापस lapic_read(LAPIC_ESR);
}

#अगर_घोषित CONFIG_SMP
अटल पूर्णांक lapic_disable_in_msr(व्योम)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	वापस 1;
}
#पूर्ण_अगर /* CONFIG_SMP */

व्योम lapic_disable(व्योम)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	अगर (!lapic_addr)
		वापस;
	
#अगर_घोषित CONFIG_SMP
	अगर (cpu_is_bsp(cpuid) && !apic_imcrp)
#पूर्ण_अगर
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_MASK|APIC_ICR_INT_MASK);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		वापस;
	}

#अगर_घोषित CONFIG_SMP
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
#पूर्ण_अगर /* CONFIG_SMP */
}

अटल पूर्णांक lapic_enable_in_msr(व्योम)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);

#अगर 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	अगर (addr != (lapic_addr >> 12)) {
		अगर (msr_hi & 0xf) {
			म_लिखो("ERROR : APIC address needs more then 32 bits\n");
			वापस 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#पूर्ण_अगर

	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);

	वापस 1;
}

पूर्णांक lapic_enable(अचिन्हित cpu)
{
	u32_t val, nlvt;

	अगर (!lapic_addr)
		वापस 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	अगर (!cpu_has_tsc) {
		म_लिखो("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		वापस 0;
	}

	अगर (!lapic_enable_in_msr())
		वापस 0;

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
	(व्योम) lapic_read(LAPIC_SIVR);

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

	अगर(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_MASK);
	}

	अगर(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_MASK);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(व्योम) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(म_लिखो("APIC timer calibrated\n"));

	वापस 1;
}

व्योम apic_spurios_intr_handler(व्योम)
{
	अटल अचिन्हित x;

	x++;
	अगर (x == 1 || (x % 100) == 0)
		म_लिखो("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

व्योम apic_error_intr_handler(व्योम)
{
	अटल अचिन्हित x;

	x++;
	अगर (x == 1 || (x % 100) == 0)
		म_लिखो("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

अटल काष्ठा gate_table_s gate_table_ioapic[] = {
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
	{ शून्य, 0, 0}
};

अटल काष्ठा gate_table_s gate_table_common[] = {
	{ ipc_entry_softint_orig, IPC_VECTOR_ORIG, USER_PRIVILEGE },
	{ kernel_call_entry_orig, KERN_CALL_VECTOR_ORIG, USER_PRIVILEGE },
	{ ipc_entry_softint_um, IPC_VECTOR_UM, USER_PRIVILEGE },
	{ kernel_call_entry_um, KERN_CALL_VECTOR_UM, USER_PRIVILEGE },
	{ शून्य, 0, 0}
};

#अगर_घोषित CONFIG_SMP
अटल काष्ठा gate_table_s gate_table_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_VECTOR, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_VECTOR, INTR_PRIVILEGE },
	{ शून्य, 0, 0}
};
#पूर्ण_अगर

#अगर_घोषित APIC_DEBUG
अटल व्योम lapic_set_dummy_handlers(व्योम)
{
	अक्षर * handler;
	पूर्णांक vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_start;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_आकार;
	क्रम(; handler < &lapic_intr_dummy_handles_end;
			handler += LAPIC_INTR_DUMMY_HANDLER_आकार) {
		int_gate_idt(vect++, (vir_bytes) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_SHIFT));
	}
}
#पूर्ण_अगर

/* Build descriptors for interrupt gates in IDT. */
व्योम apic_idt_init(स्थिर पूर्णांक reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	पूर्णांक is_bsp;

	अगर (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_table_common);
		वापस;
	}

	is_bsp = is_boot_apic(apicid());

#अगर_घोषित APIC_DEBUG
	अगर (is_bsp)
		म_लिखो("APIC debugging is enabled\n");
	lapic_set_dummy_handlers();
#पूर्ण_अगर

	/* Build descriptors for interrupt gates in IDT. */
	अगर (ioapic_enabled)
		idt_copy_vectors(gate_table_ioapic);
	अन्यथा
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_table_common);

#अगर_घोषित CONFIG_SMP
	idt_copy_vectors(gate_table_smp);
#पूर्ण_अगर

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_VECTOR;
	val &= ~ APIC_ICR_INT_MASK;
	lapic_write(LAPIC_LVTER, val);
	(व्योम) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	अगर (is_bsp) {
		BOOT_VERBOSE(म_लिखो("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_VECTOR, (vir_bytes) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_SHIFT));
	}

}

अटल पूर्णांक acpi_get_ioapics(काष्ठा io_apic * ioa, अचिन्हित * nioa, अचिन्हित max)
{
	अचिन्हित n = 0;
	काष्ठा acpi_madt_ioapic * acpi_ioa;

	जबतक (n < max) {
		acpi_ioa = acpi_get_ioapic_अग्र();
		अगर (acpi_ioa == शून्य)
			अवरोध;

		निश्चित(acpi_ioa->address);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->address;
		ioa[n].paddr = (phys_bytes) acpi_ioa->address;
		ioa[n].gsi_base = acpi_ioa->global_int_base;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_VERSION) & 0xff0000) >> 16)+1;
		म_लिखो("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	वापस n;
}

पूर्णांक detect_ioapics(व्योम)
{
	पूर्णांक status;

	अगर (machine.acpi_rsdp) {
		status = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} अन्यथा {
		status = 0;
	}
	अगर (!status) {
		/* try something different like MPS */
	}

	वापस status;
}

#अगर_घोषित CONFIG_SMP

व्योम apic_send_ipi(अचिन्हित vector, अचिन्हित cpu, पूर्णांक type)
{
	u32_t icr1, icr2;

	अगर (ncpus == 1)
		/* no need of sending an IPI */
		वापस;

	जबतक (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	चयन (type) {
		हाल APIC_IPI_DEST:
			अगर (!cpu_is_ready(cpu))
				वापस;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_FIELD | vector);
			अवरोध;
		हाल APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | vector);
			अवरोध;
		हाल APIC_IPI_TO_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | vector);
			अवरोध;
		हाल APIC_IPI_TO_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | vector);
			अवरोध;
		शेष:
			म_लिखो("WARNING : unknown send ipi type request\n");
	}

}

पूर्णांक apic_send_startup_ipi(अचिन्हित cpu, phys_bytes trampoline)
{
	पूर्णांक timeout;
	u32_t errstatus = 0;
	पूर्णांक i;

	/* INIT-SIPI-SIPI sequence */

	क्रम (i = 0; i < 2; i++) {
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

		जबतक ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			अगर (!timeout) अवरोध;
		}

		/* skip this one and continue with another cpu */
		अगर (errstatus)
			वापस -1;
	}

	वापस 0;
}

पूर्णांक apic_send_init_ipi(अचिन्हित cpu, phys_bytes trampoline) 
{
	u32_t ptr, errstatus = 0;
	पूर्णांक timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), माप(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), माप(u16_t ));

	/* set shutdown code */
	outb (RTC_सूचकांक, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(व्योम) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	जबतक ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		अगर (!timeout) अवरोध;
	}

	अगर (errstatus) 
		वापस -1; /* to continue with a new processor */

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

	जबतक ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		अगर(!timeout) अवरोध;
	}

	अगर (errstatus) 
		वापस -1; /* with the new processor */

	/* clear error state register.  */
	(व्योम) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	वापस 0;
}
#पूर्ण_अगर

#अगर_अघोषित CONFIG_SMP
पूर्णांक apic_single_cpu_init(व्योम)
{
	अगर (!cpu_feature_apic_on_chip())
		वापस 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	अगर (!lapic_enable(0)) {
		lapic_addr = 0x0;
		वापस 0;
	}

	bsp_lapic_id = apicid();
	म_लिखो("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	अगर (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		वापस 0;
	}

	ioapic_enable_all();

	अगर (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	वापस 1;
}
#पूर्ण_अगर

अटल eoi_method_t set_eoi_method(अचिन्हित irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	अगर (irq < 16)
		वापस ioapic_eoi_edge;
	अन्यथा
		वापस ioapic_eoi_level;
}

व्योम set_irq_redir_निचला(अचिन्हित irq, u32_t * निचला)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_VECTOR | APIC_ICR_INT_MASK |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	अगर (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	अन्यथा {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].vector;

	*निचला = val;
}

व्योम ioapic_set_irq(अचिन्हित irq)
{
	अचिन्हित ioa;

	निश्चित(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	अगर (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		वापस;
	
	निश्चित(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	क्रम (ioa = 0; ioa < nioapics; ioa++) {
		अगर (io_apic[ioa].gsi_base <= irq &&
				io_apic[ioa].gsi_base +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, निचला_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_base;
			io_apic_irq[irq].eoi = set_eoi_method(irq);
			io_apic_irq[irq].vector = LAPIC_VECTOR(irq);

			set_irq_redir_निचला(irq, &निचला_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_entry_write((व्योम *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, निचला_32);
		}
	}
}

व्योम ioapic_unset_irq(अचिन्हित irq)
{
	निश्चित(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = शून्य;
	io_apic_irq[irq].eoi = शून्य;
}

व्योम ioapic_reset_pic(व्योम)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

अटल व्योम irq_lapic_status(पूर्णांक irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	पूर्णांक vector;
	काष्ठा irq * intr;

	intr = &io_apic_irq[irq];
	
	अगर (!intr->ioa)
		वापस;

	vector = LAPIC_VECTOR(irq);
	tmr =  apic_read_tmr_vector(vector);
	irr =  apic_read_irr_vector(vector);
	isr =  apic_read_isr_vector(vector);


	अगर (lapic_test_delivery_val(isr, vector)) {
		म_लिखो("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, vector,
				lapic_test_delivery_val(tmr, vector) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, vector) ? 1 : 0,
				lapic_test_delivery_val(isr, vector) ? 1 : 0);
	} अन्यथा {
		म_लिखो("IRQ %d vec %d irr %d\n",
				irq, vector,
				lapic_test_delivery_val(irr, vector) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_TABLE + intr->pin * 2);
	म_लिखो("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->vector,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

व्योम dump_apic_irq_state(व्योम)
{
	पूर्णांक irq;

	म_लिखो("--- IRQs state dump ---\n");
	क्रम (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_status(irq);
	}
	म_लिखो("--- all ---\n");
}
