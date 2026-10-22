/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#ضمن <assert.h>

#ضمن <unistd.h>
#ضمن <minix/portio.h>

#ضمن <minix/syslib.h>
#ضمن <machine/cmos.h>

#ضمن <minix/u64.h>

#ضمن "apic.h"
#ضمن "apic_asm.h"
#ضمن "kernel/clock.h"
#ضمن "glo.h"
#ضمن "hw_intr.h"

#ضمن "acpi.h"

#که_تعریف_شوی USE_WATCHDOG
#ضمن "kernel/watchdog.h"
#د_که_پای

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

#تعریف_کړه IS_سېټ(ماسک)		(ماسک)
#تعریف_کړه IS_CLEAR(ماسک)		0

#تعریف_کړه APIC_LVTT_ویکټور_ماسک	0x000000FF
#تعریف_کړه APIC_LVTT_DS_PENDING	(1 << 12)
#تعریف_کړه APIC_LVTT_ماسک		(1 << 16)
#تعریف_کړه APIC_LVTT_TM		(1 << 17)

#تعریف_کړه APIC_LVT_IIPP_ماسک	0x00002000
#تعریف_کړه APIC_LVT_IIPP_AH	0x00002000
#تعریف_کړه APIC_LVT_IIPP_AL	0x00000000

#تعریف_کړه APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#تعریف_کړه APIC_LVT_TM_PERIODIC	IS_سېټ(APIC_LVTT_TM)

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
#تعریف_کړه APIC_ICR_INTPOL_ټیټ		IS_سېټ(APIC_ICR_INT_POLARITY)
#تعریف_کړه APIC_ICR_INTPOL_لوړ		IS_CLEAR(APIC_ICR_INT_POLARITY)

#تعریف_کړه APIC_ICR_LEVEL_ASSERT		(1 << 14)
#تعریف_کړه APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#تعریف_کړه APIC_ICR_TRIGGER		(1 << 15)
#تعریف_کړه APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#تعریف_کړه APIC_ICR_TM_څنډه		IS_CLEAR(APIC_ICR_TRIGGER)

#تعریف_کړه APIC_ICR_INT_ماسک		(1 << 16)

#تعریف_کړه APIC_ICR_DEST_ډګر		(0 << 18)
#تعریف_کړه APIC_ICR_DEST_SELF		(1 << 18)
#تعریف_کړه APIC_ICR_DEST_ALL		(2 << 18)
#تعریف_کړه APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#تعریف_کړه IA32_APIC_بنسټ	0x1b
#تعریف_کړه IA32_APIC_بنسټ_ENABLE_بټ	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#تعریف_کړه LAPIC_ویکټور(irq)	(IRQ0_ویکټور +(irq))

#تعریف_کړه IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#تعریف_کړه SPL0				0x0
#تعریف_کړه	SPLHI				0xF


بنية io_apic io_apic[MAX_NR_IOAPICS];
بینښه nioapics;

بنية irq;
تعريفنوع فراغ (* eoi_لاره_t)(بنية irq *);

بنية irq {
	بنية io_apic * 	ioa;
	بینښه		pin;
	بینښه		ویکټور;
	eoi_لاره_t		eoi;
	بینښه		state;
};

ساكن بنية irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#که_تعریف_شوی CONFIG_SMP

#ضمن "kernel/smp.h"

#د_که_پای

#ضمن "kernel/spinlock.h"


#تعریف_کړه lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#تعریف_کړه lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#تعریف_کړه lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#تعریف_کړه lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#تعریف_کړه is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#تعریف_کړه VERBOSE_APIC(x) x

صحيح ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_بایټونه lapic_addr;
vir_بایټونه lapic_eoi_addr;
صحيح bsp_lapic_id;

ساكن متطاير بینښه probe_ticks;
ساكن	u64_t tsc0, tsc1;
ساكن	u32_t lapic_tctr0, lapic_tctr1;

ساكن بینښه apic_imcrp;
ساكن ثابت بینښه nlints = 0;

فراغ arch_eoi(فراغ)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
ساكن u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#تعریف_کړه PROBE_TICKS	(system_hz / 10)

#تعریف_کړه IOAPIC_IOREGSEL	0x0
#تعریف_کړه IOAPIC_IOWIN	0x10

ساكن u32_t ioapic_read(u32_t ioa_بنسټ, u32_t reg)
{
	*((متطاير u32_t *)(ioa_بنسټ + IOAPIC_IOREGSEL)) = (reg & 0xff);
	أرجع *(متطاير u32_t *)(ioa_بنسټ + IOAPIC_IOWIN);
}

ساكن فراغ ioapic_write(u32_t ioa_بنسټ, u8_t reg, u32_t val)
{
	*((متطاير u32_t *)(ioa_بنسټ + IOAPIC_IOREGSEL)) = reg;
	*((متطاير u32_t *)(ioa_بنسټ + IOAPIC_IOWIN)) = val;
}

فراغ lapic_microsec_sleep(بینښه شمېر);
فراغ apic_idt_init(ثابت صحيح reset);

ساكن فراغ ioapic_enable_pin(vir_بایټونه ioapic_addr, صحيح pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2);

	lo &= ~APIC_ICR_INT_ماسک;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2, lo);
}

ساكن فراغ ioapic_disable_pin(vir_بایټونه ioapic_addr, صحيح pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2);

	lo |= APIC_ICR_INT_ماسک;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2, lo);
}

#إذا 0
ساكن فراغ ioapic_redirt_ثبت_read(فراغ * ioapic_addr,
					صحيح ثبت,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + ثبت * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + ثبت * 2 + 1));

}
#د_که_پای

ساكن فراغ ioapic_redirt_ثبت_write(فراغ * ioapic_addr,
					صحيح ثبت,
					u32_t hi,
					u32_t lo)
{
#إذا 0
	VERBOSE_APIC(چاپ_کړه("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", ثبت, hi, lo));
#د_که_پای
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + ثبت * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + ثبت * 2), lo);
}

#تعریف_کړه apic_read_tmr_ویکټور(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#تعریف_کړه apic_read_irr_ویکټور(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#تعریف_کړه apic_read_isr_ویکټور(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#تعریف_کړه lapic_test_delivery_val(val, ویکټور) ((val) & (1 << ((ویکټور) & 0x1f)))

ساكن فراغ ioapic_eoi_level(بنية irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_ویکټور(irq->ویکټور);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	إذا (!lapic_test_delivery_val(tmr, irq->ویکټور)) {
		صحيح is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_ماسک;

		/* set mask and edge */
		lo |= APIC_ICR_INT_ماسک;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		إذا (is_masked)
			lo |= APIC_ICR_INT_ماسک;
		وإلا
			lo &= ~APIC_ICR_INT_ماسک;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2, lo);
	}
}

ساكن فراغ ioapic_eoi_څنډه(__unused بنية irq * irq)
{
	apic_eoi();
}

فراغ ioapic_eoi(صحيح irq)
{
	إذا (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	وإلا
		irq_8259_eoi(irq);
}
 
فراغ ioapic_سېټ_id(u32_t addr, بینښه صحيح id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

صحيح ioapic_enable_all(فراغ)
{
	i8259_disable();

	إذا (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	أرجع ioapic_enabled = 1;
}

/* disables a single IO APIC */
ساكن فراغ ioapic_disable(بنية io_apic * ioapic)
{
	بینښه p;
	
	لكل (p = 0; p < io_apic->pins; p++) {
		u32_t ټیټ_32, hi_32;
		ټیټ_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2 + 1));

		إذا (!(ټیټ_32 & APIC_ICR_INT_ماسک)) {
			ټیټ_32 |= APIC_ICR_INT_ماسک;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2), ټیټ_32);
		}
	}
}

/* disables all IO APICs */
فراغ ioapic_disable_all(فراغ)
{
	بینښه ioa;
	إذا (!ioapic_enabled)
		أرجع;

	لكل (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	إذا (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

ساكن فراغ ioapic_disable_irq(بینښه irq)
{
	إذا(!(io_apic_irq[irq].ioa)) {
		چاپ_کړه("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		أرجع;
	}

	أكد(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

ساكن فراغ ioapic_enable_irq(بینښه irq)
{
	إذا(!(io_apic_irq[irq].ioa)) {
		چاپ_کړه("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		أرجع;
	}

	أكد(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

فراغ ioapic_unmask_irq(بینښه irq)
{
	إذا (ioapic_enabled)
		ioapic_enable_irq(irq);
	وإلا
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

فراغ ioapic_ماسک_irq(بینښه irq)
{
	إذا (ioapic_enabled)
		ioapic_disable_irq(irq);
	وإلا
		/* FIXME unlikely */
		irq_8259_ماسک(irq);
}

بینښه صحيح apicid(فراغ)
{
	أرجع lapic_read(LAPIC_ID) >> 24;
}

ساكن صحيح calib_clk_handler(irq_hook_t * ناکارول_شوی(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	إذا (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	وإلا إذا (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	أرجع 1;
}

ساكن صحيح spurious_irq_handler(irq_hook_t * ناکارول_شوی(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	أرجع 1;
}

ساكن فراغ apic_calibrate_clocks(بینښه cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(چاپ_کړه("Calibrating clock\n"));
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
	lvtt |= APIC_LVTT_ماسک;
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
	طالما(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_قلف();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(چاپ_کړه("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_سېټ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (بینښه اوږد)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

فراغ lapic_سېټ_timer_one_shot(ثابت u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_فی_us;
	ثابت u8_t cpu = cpuid;

	ticks_فی_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_فی_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_ویکټور;
	lapic_write(LAPIC_LVTTR, lvtt);
}

فراغ lapic_سېټ_timer_periodic(ثابت بینښه freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_فی_clock_tick;
	ثابت u8_t cpu = cpuid;

	lapic_ticks_فی_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_ویکټور;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_فی_clock_tick);
}

فراغ lapic_stop_timer(فراغ)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_ماسک);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

فراغ lapic_restart_timer(فراغ)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	إذا (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_سېټ_timer_one_shot(1000000/system_hz);
}

فراغ lapic_microsec_sleep(بینښه شمېر)
{
	lapic_سېټ_timer_one_shot(شمېر);
	طالما (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

ساكن  u32_t lapic_errstatus(فراغ)
{
	lapic_write(LAPIC_ESR, 0);
	أرجع lapic_read(LAPIC_ESR);
}

#که_تعریف_شوی CONFIG_SMP
ساكن صحيح lapic_disable_دننه_msr(فراغ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_بنسټ, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_بنسټ_ENABLE_بټ);
	ia32_msr_write(IA32_APIC_بنسټ, msr_hi, msr_lo);

	أرجع 1;
}
#د_که_پای /* CONFIG_SMP */

فراغ lapic_disable(فراغ)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	إذا (!lapic_addr)
		أرجع;
	
#که_تعریف_شوی CONFIG_SMP
	إذا (cpu_is_bsp(cpuid) && !apic_imcrp)
#د_که_پای
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_ماسک|APIC_ICR_INT_ماسک);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		أرجع;
	}

#که_تعریف_شوی CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ماسک;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ماسک;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_دننه_msr();
#د_که_پای /* CONFIG_SMP */
}

ساكن صحيح lapic_enable_دننه_msr(فراغ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_بنسټ, &msr_hi, &msr_lo);

#إذا 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	إذا (addr != (lapic_addr >> 12)) {
		إذا (msr_hi & 0xf) {
			چاپ_کړه("ERROR : APIC address needs more then 32 bits\n");
			أرجع 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#د_که_پای

	msr_lo |= (1 << IA32_APIC_بنسټ_ENABLE_بټ);
	ia32_msr_write(IA32_APIC_بنسټ, msr_hi, msr_lo);

	أرجع 1;
}

صحيح lapic_enable(بینښه cpu)
{
	u32_t val, nlvt;

	إذا (!lapic_addr)
		أرجع 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	إذا (!cpu_has_tsc) {
		چاپ_کړه("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		أرجع 0;
	}

	إذا (!lapic_enable_دننه_msr())
		أرجع 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_ویکټور;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(فراغ) lapic_read(LAPIC_SIVR);

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

	nlvt = (lapic_read(LAPIC_نسخه)>>16) & 0xFF;

	إذا(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_ماسک);
	}

	إذا(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_ماسک);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(فراغ) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(چاپ_کړه("APIC timer calibrated\n"));

	أرجع 1;
}

فراغ apic_spurios_intr_handler(فراغ)
{
	ساكن بینښه x;

	x++;
	إذا (x == 1 || (x % 100) == 0)
		چاپ_کړه("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

فراغ apic_error_intr_handler(فراغ)
{
	ساكن بینښه x;

	x++;
	إذا (x == 1 || (x % 100) == 0)
		چاپ_کړه("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

ساكن بنية gate_جدول_s gate_جدول_ioapic[] = {
	{ apic_hwint0, LAPIC_ویکټور( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_ویکټور( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_ویکټور( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_ویکټور( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_ویکټور( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_ویکټور( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_ویکټور( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_ویکټور( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_ویکټور( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_ویکټور( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_ویکټور(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_ویکټور(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_ویکټور(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_ویکټور(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_ویکټور(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_ویکټور(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_ویکټور(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_ویکټور(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_ویکټور(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_ویکټور(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_ویکټور(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_ویکټور(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_ویکټور(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_ویکټور(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_ویکټور(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_ویکټور(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_ویکټور(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_ویکټور(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_ویکټور(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_ویکټور(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_ویکټور(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_ویکټور(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_ویکټور(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_ویکټور(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_ویکټور(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_ویکټور(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_ویکټور(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_ویکټور(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_ویکټور(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_ویکټور(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_ویکټور(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_ویکټور(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_ویکټور(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_ویکټور(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_ویکټور(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_ویکټور(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_ویکټور(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_ویکټور(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_ویکټور(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_ویکټور(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_ویکټور(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_ویکټور(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_ویکټور(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_ویکټور(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_ویکټور(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_ویکټور(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_ویکټور(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_ویکټور(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_ویکټور(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_ویکټور(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_ویکټور(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_ویکټور(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_ویکټور(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_ویکټور(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_ویکټور, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_ویکټور, INTR_PRIVILEGE },
	{ عدم, 0, 0}
};

ساكن بنية gate_جدول_s gate_جدول_common[] = {
	{ ipc_ثبت_softint_orig, IPC_ویکټور_ORIG, USER_PRIVILEGE },
	{ kernel_call_ثبت_orig, KERN_CALL_ویکټور_ORIG, USER_PRIVILEGE },
	{ ipc_ثبت_softint_um, IPC_ویکټور_UM, USER_PRIVILEGE },
	{ kernel_call_ثبت_um, KERN_CALL_ویکټور_UM, USER_PRIVILEGE },
	{ عدم, 0, 0}
};

#که_تعریف_شوی CONFIG_SMP
ساكن بنية gate_جدول_s gate_جدول_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_ویکټور, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_ویکټور, INTR_PRIVILEGE },
	{ عدم, 0, 0}
};
#د_که_پای

#که_تعریف_شوی APIC_DEBUG
ساكن فراغ lapic_سېټ_dummy_handlers(فراغ)
{
	حرف * handler;
	صحيح vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_پیل;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_اندازه;
	لكل(; handler < &lapic_intr_dummy_handles_پای_ټکی;
			handler += LAPIC_INTR_DUMMY_HANDLER_اندازه) {
		int_gate_idt(vect++, (vir_بایټونه) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_ښويدنه));
	}
}
#د_که_پای

/* Build descriptors for interrupt gates in IDT. */
فراغ apic_idt_init(ثابت صحيح reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	صحيح is_bsp;

	إذا (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_جدول_common);
		أرجع;
	}

	is_bsp = is_boot_apic(apicid());

#که_تعریف_شوی APIC_DEBUG
	إذا (is_bsp)
		چاپ_کړه("APIC debugging is enabled\n");
	lapic_سېټ_dummy_handlers();
#د_که_پای

	/* Build descriptors for interrupt gates in IDT. */
	إذا (ioapic_enabled)
		idt_copy_vectors(gate_جدول_ioapic);
	وإلا
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_جدول_common);

#که_تعریف_شوی CONFIG_SMP
	idt_copy_vectors(gate_جدول_smp);
#د_که_پای

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_ویکټور;
	val &= ~ APIC_ICR_INT_ماسک;
	lapic_write(LAPIC_LVTER, val);
	(فراغ) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	إذا (is_bsp) {
		BOOT_VERBOSE(چاپ_کړه("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_ویکټور, (vir_بایټونه) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_ښويدنه));
	}

}

ساكن صحيح acpi_get_ioapics(بنية io_apic * ioa, بینښه * nioa, بینښه max)
{
	بینښه n = 0;
	بنية acpi_madt_ioapic * acpi_ioa;

	طالما (n < max) {
		acpi_ioa = acpi_get_ioapic_بل();
		إذا (acpi_ioa == عدم)
			اكسر;

		أكد(acpi_ioa->پته);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->پته;
		ioa[n].paddr = (phys_بایټونه) acpi_ioa->پته;
		ioa[n].gsi_بنسټ = acpi_ioa->global_int_بنسټ;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_نسخه) & 0xff0000) >> 16)+1;
		چاپ_کړه("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	أرجع n;
}

صحيح detect_ioapics(فراغ)
{
	صحيح حالت;

	إذا (machine.acpi_rsdp) {
		حالت = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} وإلا {
		حالت = 0;
	}
	إذا (!حالت) {
		/* try something different like MPS */
	}

	أرجع حالت;
}

#که_تعریف_شوی CONFIG_SMP

فراغ apic_send_ipi(بینښه ویکټور, بینښه cpu, صحيح type)
{
	u32_t icr1, icr2;

	إذا (ncpus == 1)
		/* no need of sending an IPI */
		أرجع;

	طالما (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	بدل (type) {
		حالة APIC_IPI_DEST:
			إذا (!cpu_is_ready(cpu))
				أرجع;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ډګر | ویکټور);
			اكسر;
		حالة APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | ویکټور);
			اكسر;
		حالة APIC_IPI_منزل_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | ویکټور);
			اكسر;
		حالة APIC_IPI_منزل_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | ویکټور);
			اكسر;
		افتراضي:
			چاپ_کړه("WARNING : unknown send ipi type request\n");
	}

}

صحيح apic_send_startup_ipi(بینښه cpu, phys_بایټونه trampoline)
{
	صحيح timeout;
	u32_t errstatus = 0;
	صحيح i;

	/* INIT-SIPI-SIPI sequence */

	لكل (i = 0; i < 2; i++) {
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

		طالما ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			إذا (!timeout) اكسر;
		}

		/* skip this one and continue with another cpu */
		إذا (errstatus)
			أرجع -1;
	}

	أرجع 0;
}

صحيح apic_send_init_ipi(بینښه cpu, phys_بایټونه trampoline) 
{
	u32_t ptr, errstatus = 0;
	صحيح timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), حجم(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), حجم(u16_t ));

	/* set shutdown code */
	outb (RTC_شاخص, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(فراغ) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	طالما ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		إذا (!timeout) اكسر;
	}

	إذا (errstatus) 
		أرجع -1; /* to continue with a new processor */

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

	طالما ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		إذا(!timeout) اكسر;
	}

	إذا (errstatus) 
		أرجع -1; /* with the new processor */

	/* clear error state register.  */
	(فراغ) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	أرجع 0;
}
#د_که_پای

#که_نه_تعریف_شوی CONFIG_SMP
صحيح apic_single_cpu_init(فراغ)
{
	إذا (!cpu_feature_apic_on_chip())
		أرجع 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	إذا (!lapic_enable(0)) {
		lapic_addr = 0x0;
		أرجع 0;
	}

	bsp_lapic_id = apicid();
	چاپ_کړه("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	إذا (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		أرجع 0;
	}

	ioapic_enable_all();

	إذا (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	أرجع 1;
}
#د_که_پای

ساكن eoi_لاره_t سېټ_eoi_لاره(بینښه irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	إذا (irq < 16)
		أرجع ioapic_eoi_څنډه;
	وإلا
		أرجع ioapic_eoi_level;
}

فراغ سېټ_irq_redir_ټیټ(بینښه irq, u32_t * ټیټ)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_ویکټور | APIC_ICR_INT_ماسک |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	إذا (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	وإلا {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].ویکټور;

	*ټیټ = val;
}

فراغ ioapic_سېټ_irq(بینښه irq)
{
	بینښه ioa;

	أكد(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	إذا (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		أرجع;
	
	أكد(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	لكل (ioa = 0; ioa < nioapics; ioa++) {
		إذا (io_apic[ioa].gsi_بنسټ <= irq &&
				io_apic[ioa].gsi_بنسټ +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, ټیټ_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_بنسټ;
			io_apic_irq[irq].eoi = سېټ_eoi_لاره(irq);
			io_apic_irq[irq].ویکټور = LAPIC_ویکټور(irq);

			سېټ_irq_redir_ټیټ(irq, &ټیټ_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_ثبت_write((فراغ *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, ټیټ_32);
		}
	}
}

فراغ ioapic_unset_irq(بینښه irq)
{
	أكد(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = عدم;
	io_apic_irq[irq].eoi = عدم;
}

فراغ ioapic_reset_pic(فراغ)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

ساكن فراغ irq_lapic_حالت(صحيح irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	صحيح ویکټور;
	بنية irq * intr;

	intr = &io_apic_irq[irq];
	
	إذا (!intr->ioa)
		أرجع;

	ویکټور = LAPIC_ویکټور(irq);
	tmr =  apic_read_tmr_ویکټور(ویکټور);
	irr =  apic_read_irr_ویکټور(ویکټور);
	isr =  apic_read_isr_ویکټور(ویکټور);


	إذا (lapic_test_delivery_val(isr, ویکټور)) {
		چاپ_کړه("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, ویکټور,
				lapic_test_delivery_val(tmr, ویکټور) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, ویکټور) ? 1 : 0,
				lapic_test_delivery_val(isr, ویکټور) ? 1 : 0);
	} وإلا {
		چاپ_کړه("IRQ %d vec %d irr %d\n",
				irq, ویکټور,
				lapic_test_delivery_val(irr, ویکټور) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_جدول + intr->pin * 2);
	چاپ_کړه("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->ویکټور,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

فراغ dump_apic_irq_state(فراغ)
{
	صحيح irq;

	چاپ_کړه("--- IRQs state dump ---\n");
	لكل (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_حالت(irq);
	}
	چاپ_کړه("--- all ---\n");
}
