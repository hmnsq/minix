/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#شامل <assert.h>

#شامل <unistd.h>
#شامل <minix/portio.h>

#شامل <minix/syslib.h>
#شامل <machine/cmos.h>

#شامل <minix/u64.h>

#شامل "apic.h"
#شامل "apic_asm.h"
#شامل "kernel/clock.h"
#شامل "glo.h"
#شامل "hw_intr.h"

#شامل "acpi.h"

#اگر_تعریف_شدہ USE_WATCHDOG
#شامل "kernel/watchdog.h"
#اگر_ختم

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

#تعریف_کرو IS_سیٹ(نقاب)		(نقاب)
#تعریف_کرو IS_CLEAR(نقاب)		0

#تعریف_کرو APIC_LVTT_سمتیہ_نقاب	0x000000FF
#تعریف_کرو APIC_LVTT_DS_PENDING	(1 << 12)
#تعریف_کرو APIC_LVTT_نقاب		(1 << 16)
#تعریف_کرو APIC_LVTT_TM		(1 << 17)

#تعریف_کرو APIC_LVT_IIPP_نقاب	0x00002000
#تعریف_کرو APIC_LVT_IIPP_AH	0x00002000
#تعریف_کرو APIC_LVT_IIPP_AL	0x00000000

#تعریف_کرو APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#تعریف_کرو APIC_LVT_TM_PERIODIC	IS_سیٹ(APIC_LVTT_TM)

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
#تعریف_کرو APIC_ICR_INTPOL_کم		IS_سیٹ(APIC_ICR_INT_POLARITY)
#تعریف_کرو APIC_ICR_INTPOL_بلند		IS_CLEAR(APIC_ICR_INT_POLARITY)

#تعریف_کرو APIC_ICR_LEVEL_ASSERT		(1 << 14)
#تعریف_کرو APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#تعریف_کرو APIC_ICR_TRIGGER		(1 << 15)
#تعریف_کرو APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#تعریف_کرو APIC_ICR_TM_کنارہ		IS_CLEAR(APIC_ICR_TRIGGER)

#تعریف_کرو APIC_ICR_INT_نقاب		(1 << 16)

#تعریف_کرو APIC_ICR_DEST_میدان		(0 << 18)
#تعریف_کرو APIC_ICR_DEST_SELF		(1 << 18)
#تعریف_کرو APIC_ICR_DEST_ALL		(2 << 18)
#تعریف_کرو APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#تعریف_کرو IA32_APIC_بنیاد	0x1b
#تعریف_کرو IA32_APIC_بنیاد_ENABLE_ثنائی_ہندسہ	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#تعریف_کرو LAPIC_سمتیہ(irq)	(IRQ0_سمتیہ +(irq))

#تعریف_کرو IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#تعریف_کرو SPL0				0x0
#تعریف_کرو	SPLHI				0xF


ساخت io_apic io_apic[MAX_NR_IOAPICS];
غیرعلامتی nioapics;

ساخت irq;
قسمتعریف خالی (* eoi_طریقہ_t)(ساخت irq *);

ساخت irq {
	ساخت io_apic * 	ioa;
	غیرعلامتی		pin;
	غیرعلامتی		سمتیہ;
	eoi_طریقہ_t		eoi;
	غیرعلامتی		state;
};

ساکن ساخت irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#اگر_تعریف_شدہ CONFIG_SMP

#شامل "kernel/smp.h"

#اگر_ختم

#شامل "kernel/spinlock.h"


#تعریف_کرو lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#تعریف_کرو lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#تعریف_کرو lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#تعریف_کرو lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#تعریف_کرو is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#تعریف_کرو VERBOSE_APIC(x) x

صحیح ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_بائٹیں lapic_addr;
vir_بائٹیں lapic_eoi_addr;
صحیح bsp_lapic_id;

ساکن متلون غیرعلامتی probe_ticks;
ساکن	u64_t tsc0, tsc1;
ساکن	u32_t lapic_tctr0, lapic_tctr1;

ساکن غیرعلامتی apic_imcrp;
ساکن ثابت غیرعلامتی nlints = 0;

خالی arch_eoi(خالی)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
ساکن u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#تعریف_کرو PROBE_TICKS	(system_hz / 10)

#تعریف_کرو IOAPIC_IOREGSEL	0x0
#تعریف_کرو IOAPIC_IOWIN	0x10

ساکن u32_t ioapic_read(u32_t ioa_بنیاد, u32_t reg)
{
	*((متلون u32_t *)(ioa_بنیاد + IOAPIC_IOREGSEL)) = (reg & 0xff);
	واپس *(متلون u32_t *)(ioa_بنیاد + IOAPIC_IOWIN);
}

ساکن خالی ioapic_write(u32_t ioa_بنیاد, u8_t reg, u32_t val)
{
	*((متلون u32_t *)(ioa_بنیاد + IOAPIC_IOREGSEL)) = reg;
	*((متلون u32_t *)(ioa_بنیاد + IOAPIC_IOWIN)) = val;
}

خالی lapic_microsec_sleep(غیرعلامتی گنتی);
خالی apic_idt_init(ثابت صحیح reset);

ساکن خالی ioapic_enable_pin(vir_بائٹیں ioapic_addr, صحیح pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2);

	lo &= ~APIC_ICR_INT_نقاب;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2, lo);
}

ساکن خالی ioapic_disable_pin(vir_بائٹیں ioapic_addr, صحیح pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2);

	lo |= APIC_ICR_INT_نقاب;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_جدول + pin * 2, lo);
}

#اگر 0
ساکن خالی ioapic_redirt_اندراج_read(خالی * ioapic_addr,
					صحیح اندراج_2,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + اندراج_2 * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + اندراج_2 * 2 + 1));

}
#اگر_ختم

ساکن خالی ioapic_redirt_اندراج_write(خالی * ioapic_addr,
					صحیح اندراج_2,
					u32_t hi,
					u32_t lo)
{
#اگر 0
	VERBOSE_APIC(لکھو("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", اندراج_2, hi, lo));
#اگر_ختم
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + اندراج_2 * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_جدول + اندراج_2 * 2), lo);
}

#تعریف_کرو apic_read_tmr_سمتیہ(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#تعریف_کرو apic_read_irr_سمتیہ(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#تعریف_کرو apic_read_isr_سمتیہ(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#تعریف_کرو lapic_test_delivery_val(val, سمتیہ) ((val) & (1 << ((سمتیہ) & 0x1f)))

ساکن خالی ioapic_eoi_level(ساخت irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_سمتیہ(irq->سمتیہ);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	اگر (!lapic_test_delivery_val(tmr, irq->سمتیہ)) {
		صحیح is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_نقاب;

		/* set mask and edge */
		lo |= APIC_ICR_INT_نقاب;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		اگر (is_masked)
			lo |= APIC_ICR_INT_نقاب;
		ورنہ
			lo &= ~APIC_ICR_INT_نقاب;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_جدول + irq->pin * 2, lo);
	}
}

ساکن خالی ioapic_eoi_کنارہ(__unused ساخت irq * irq)
{
	apic_eoi();
}

خالی ioapic_eoi(صحیح irq)
{
	اگر (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	ورنہ
		irq_8259_eoi(irq);
}
 
خالی ioapic_سیٹ_id(u32_t addr, غیرعلامتی صحیح id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

صحیح ioapic_enable_all(خالی)
{
	i8259_disable();

	اگر (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	واپس ioapic_enabled = 1;
}

/* disables a single IO APIC */
ساکن خالی ioapic_disable(ساخت io_apic * ioapic)
{
	غیرعلامتی p;
	
	برائے (p = 0; p < io_apic->pins; p++) {
		u32_t کم_32, hi_32;
		کم_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2 + 1));

		اگر (!(کم_32 & APIC_ICR_INT_نقاب)) {
			کم_32 |= APIC_ICR_INT_نقاب;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_جدول + p * 2), کم_32);
		}
	}
}

/* disables all IO APICs */
خالی ioapic_disable_all(خالی)
{
	غیرعلامتی ioa;
	اگر (!ioapic_enabled)
		واپس;

	برائے (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	اگر (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

ساکن خالی ioapic_disable_irq(غیرعلامتی irq)
{
	اگر(!(io_apic_irq[irq].ioa)) {
		لکھو("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		واپس;
	}

	أكد(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

ساکن خالی ioapic_enable_irq(غیرعلامتی irq)
{
	اگر(!(io_apic_irq[irq].ioa)) {
		لکھو("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		واپس;
	}

	أكد(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

خالی ioapic_unmask_irq(غیرعلامتی irq)
{
	اگر (ioapic_enabled)
		ioapic_enable_irq(irq);
	ورنہ
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

خالی ioapic_نقاب_irq(غیرعلامتی irq)
{
	اگر (ioapic_enabled)
		ioapic_disable_irq(irq);
	ورنہ
		/* FIXME unlikely */
		irq_8259_نقاب(irq);
}

غیرعلامتی صحیح apicid(خالی)
{
	واپس lapic_read(LAPIC_ID) >> 24;
}

ساکن صحیح calib_clk_handler(irq_hook_t * غیر_مستعمل(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	اگر (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	ورنہ اگر (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	واپس 1;
}

ساکن صحیح spurious_irq_handler(irq_hook_t * غیر_مستعمل(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	واپس 1;
}

ساکن خالی apic_calibrate_clocks(غیرعلامتی cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(لکھو("Calibrating clock\n"));
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
	lvtt |= APIC_LVTT_نقاب;
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
	جب(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_تالا();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(لکھو("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_سیٹ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (غیرعلامتی طویل)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

خالی lapic_سیٹ_timer_one_shot(ثابت u32_t usec)
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
	lvtt = APIC_TIMER_INT_سمتیہ;
	lapic_write(LAPIC_LVTTR, lvtt);
}

خالی lapic_سیٹ_timer_periodic(ثابت غیرعلامتی freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_فی_clock_tick;
	ثابت u8_t cpu = cpuid;

	lapic_ticks_فی_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_سمتیہ;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_فی_clock_tick);
}

خالی lapic_stop_timer(خالی)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_نقاب);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

خالی lapic_restart_timer(خالی)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	اگر (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_سیٹ_timer_one_shot(1000000/system_hz);
}

خالی lapic_microsec_sleep(غیرعلامتی گنتی)
{
	lapic_سیٹ_timer_one_shot(گنتی);
	جب (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

ساکن  u32_t lapic_errstatus(خالی)
{
	lapic_write(LAPIC_ESR, 0);
	واپس lapic_read(LAPIC_ESR);
}

#اگر_تعریف_شدہ CONFIG_SMP
ساکن صحیح lapic_disable_اندر_msr(خالی)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_بنیاد, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_بنیاد_ENABLE_ثنائی_ہندسہ);
	ia32_msr_write(IA32_APIC_بنیاد, msr_hi, msr_lo);

	واپس 1;
}
#اگر_ختم /* CONFIG_SMP */

خالی lapic_disable(خالی)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	اگر (!lapic_addr)
		واپس;
	
#اگر_تعریف_شدہ CONFIG_SMP
	اگر (cpu_is_bsp(cpuid) && !apic_imcrp)
#اگر_ختم
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_نقاب|APIC_ICR_INT_نقاب);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		واپس;
	}

#اگر_تعریف_شدہ CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_نقاب;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_نقاب;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_اندر_msr();
#اگر_ختم /* CONFIG_SMP */
}

ساکن صحیح lapic_enable_اندر_msr(خالی)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_بنیاد, &msr_hi, &msr_lo);

#اگر 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	اگر (addr != (lapic_addr >> 12)) {
		اگر (msr_hi & 0xf) {
			لکھو("ERROR : APIC address needs more then 32 bits\n");
			واپس 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#اگر_ختم

	msr_lo |= (1 << IA32_APIC_بنیاد_ENABLE_ثنائی_ہندسہ);
	ia32_msr_write(IA32_APIC_بنیاد, msr_hi, msr_lo);

	واپس 1;
}

صحیح lapic_enable(غیرعلامتی cpu)
{
	u32_t val, nlvt;

	اگر (!lapic_addr)
		واپس 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	اگر (!cpu_has_tsc) {
		لکھو("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		واپس 0;
	}

	اگر (!lapic_enable_اندر_msr())
		واپس 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_سمتیہ;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(خالی) lapic_read(LAPIC_SIVR);

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

	nlvt = (lapic_read(LAPIC_نسخہ)>>16) & 0xFF;

	اگر(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_نقاب);
	}

	اگر(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_نقاب);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(خالی) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(لکھو("APIC timer calibrated\n"));

	واپس 1;
}

خالی apic_spurios_intr_handler(خالی)
{
	ساکن غیرعلامتی x;

	x++;
	اگر (x == 1 || (x % 100) == 0)
		لکھو("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

خالی apic_error_intr_handler(خالی)
{
	ساکن غیرعلامتی x;

	x++;
	اگر (x == 1 || (x % 100) == 0)
		لکھو("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

ساکن ساخت gate_جدول_s gate_جدول_ioapic[] = {
	{ apic_hwint0, LAPIC_سمتیہ( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_سمتیہ( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_سمتیہ( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_سمتیہ( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_سمتیہ( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_سمتیہ( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_سمتیہ( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_سمتیہ( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_سمتیہ( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_سمتیہ( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_سمتیہ(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_سمتیہ(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_سمتیہ(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_سمتیہ(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_سمتیہ(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_سمتیہ(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_سمتیہ(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_سمتیہ(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_سمتیہ(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_سمتیہ(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_سمتیہ(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_سمتیہ(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_سمتیہ(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_سمتیہ(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_سمتیہ(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_سمتیہ(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_سمتیہ(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_سمتیہ(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_سمتیہ(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_سمتیہ(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_سمتیہ(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_سمتیہ(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_سمتیہ(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_سمتیہ(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_سمتیہ(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_سمتیہ(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_سمتیہ(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_سمتیہ(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_سمتیہ(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_سمتیہ(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_سمتیہ(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_سمتیہ(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_سمتیہ(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_سمتیہ(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_سمتیہ(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_سمتیہ(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_سمتیہ(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_سمتیہ(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_سمتیہ(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_سمتیہ(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_سمتیہ(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_سمتیہ(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_سمتیہ(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_سمتیہ(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_سمتیہ(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_سمتیہ(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_سمتیہ(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_سمتیہ(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_سمتیہ(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_سمتیہ(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_سمتیہ(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_سمتیہ(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_سمتیہ(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_سمتیہ(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_سمتیہ, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_سمتیہ, INTR_PRIVILEGE },
	{ تہی, 0, 0}
};

ساکن ساخت gate_جدول_s gate_جدول_common[] = {
	{ ipc_اندراج_softint_orig, IPC_سمتیہ_ORIG, USER_PRIVILEGE },
	{ kernel_call_اندراج_orig, KERN_CALL_سمتیہ_ORIG, USER_PRIVILEGE },
	{ ipc_اندراج_softint_um, IPC_سمتیہ_UM, USER_PRIVILEGE },
	{ kernel_call_اندراج_um, KERN_CALL_سمتیہ_UM, USER_PRIVILEGE },
	{ تہی, 0, 0}
};

#اگر_تعریف_شدہ CONFIG_SMP
ساکن ساخت gate_جدول_s gate_جدول_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_سمتیہ, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_سمتیہ, INTR_PRIVILEGE },
	{ تہی, 0, 0}
};
#اگر_ختم

#اگر_تعریف_شدہ APIC_DEBUG
ساکن خالی lapic_سیٹ_dummy_handlers(خالی)
{
	حرف * handler;
	صحیح vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_آغاز;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_سائز;
	برائے(; handler < &lapic_intr_dummy_handles_انتہا;
			handler += LAPIC_INTR_DUMMY_HANDLER_سائز) {
		int_gate_idt(vect++, (vir_بائٹیں) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_سرکاؤ));
	}
}
#اگر_ختم

/* Build descriptors for interrupt gates in IDT. */
خالی apic_idt_init(ثابت صحیح reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	صحیح is_bsp;

	اگر (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_جدول_common);
		واپس;
	}

	is_bsp = is_boot_apic(apicid());

#اگر_تعریف_شدہ APIC_DEBUG
	اگر (is_bsp)
		لکھو("APIC debugging is enabled\n");
	lapic_سیٹ_dummy_handlers();
#اگر_ختم

	/* Build descriptors for interrupt gates in IDT. */
	اگر (ioapic_enabled)
		idt_copy_vectors(gate_جدول_ioapic);
	ورنہ
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_جدول_common);

#اگر_تعریف_شدہ CONFIG_SMP
	idt_copy_vectors(gate_جدول_smp);
#اگر_ختم

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_سمتیہ;
	val &= ~ APIC_ICR_INT_نقاب;
	lapic_write(LAPIC_LVTER, val);
	(خالی) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	اگر (is_bsp) {
		BOOT_VERBOSE(لکھو("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_سمتیہ, (vir_بائٹیں) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_سرکاؤ));
	}

}

ساکن صحیح acpi_get_ioapics(ساخت io_apic * ioa, غیرعلامتی * nioa, غیرعلامتی max)
{
	غیرعلامتی n = 0;
	ساخت acpi_madt_ioapic * acpi_ioa;

	جب (n < max) {
		acpi_ioa = acpi_get_ioapic_اگلا();
		اگر (acpi_ioa == تہی)
			توڑو;

		أكد(acpi_ioa->پتہ);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->پتہ;
		ioa[n].paddr = (phys_بائٹیں) acpi_ioa->پتہ;
		ioa[n].gsi_بنیاد = acpi_ioa->global_int_بنیاد;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_نسخہ) & 0xff0000) >> 16)+1;
		لکھو("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	واپس n;
}

صحیح detect_ioapics(خالی)
{
	صحیح حالت_2;

	اگر (machine.acpi_rsdp) {
		حالت_2 = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} ورنہ {
		حالت_2 = 0;
	}
	اگر (!حالت_2) {
		/* try something different like MPS */
	}

	واپس حالت_2;
}

#اگر_تعریف_شدہ CONFIG_SMP

خالی apic_send_ipi(غیرعلامتی سمتیہ, غیرعلامتی cpu, صحیح type)
{
	u32_t icr1, icr2;

	اگر (ncpus == 1)
		/* no need of sending an IPI */
		واپس;

	جب (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	انتخاب (type) {
		حالت APIC_IPI_DEST:
			اگر (!cpu_is_ready(cpu))
				واپس;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_میدان | سمتیہ);
			توڑو;
		حالت APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | سمتیہ);
			توڑو;
		حالت APIC_IPI_مقصد_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | سمتیہ);
			توڑو;
		حالت APIC_IPI_مقصد_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | سمتیہ);
			توڑو;
		طےشدہ:
			لکھو("WARNING : unknown send ipi type request\n");
	}

}

صحیح apic_send_startup_ipi(غیرعلامتی cpu, phys_بائٹیں trampoline)
{
	صحیح timeout;
	u32_t errstatus = 0;
	صحیح i;

	/* INIT-SIPI-SIPI sequence */

	برائے (i = 0; i < 2; i++) {
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

		جب ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			اگر (!timeout) توڑو;
		}

		/* skip this one and continue with another cpu */
		اگر (errstatus)
			واپس -1;
	}

	واپس 0;
}

صحیح apic_send_init_ipi(غیرعلامتی cpu, phys_بائٹیں trampoline) 
{
	u32_t ptr, errstatus = 0;
	صحیح timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), حجم(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), حجم(u16_t ));

	/* set shutdown code */
	outb (RTC_اشاریہ, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(خالی) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	جب ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		اگر (!timeout) توڑو;
	}

	اگر (errstatus) 
		واپس -1; /* to continue with a new processor */

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

	جب ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		اگر(!timeout) توڑو;
	}

	اگر (errstatus) 
		واپس -1; /* with the new processor */

	/* clear error state register.  */
	(خالی) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	واپس 0;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ CONFIG_SMP
صحیح apic_single_cpu_init(خالی)
{
	اگر (!cpu_feature_apic_on_chip())
		واپس 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	اگر (!lapic_enable(0)) {
		lapic_addr = 0x0;
		واپس 0;
	}

	bsp_lapic_id = apicid();
	لکھو("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	اگر (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		واپس 0;
	}

	ioapic_enable_all();

	اگر (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	واپس 1;
}
#اگر_ختم

ساکن eoi_طریقہ_t سیٹ_eoi_طریقہ(غیرعلامتی irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	اگر (irq < 16)
		واپس ioapic_eoi_کنارہ;
	ورنہ
		واپس ioapic_eoi_level;
}

خالی سیٹ_irq_redir_کم(غیرعلامتی irq, u32_t * کم)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_سمتیہ | APIC_ICR_INT_نقاب |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	اگر (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	ورنہ {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].سمتیہ;

	*کم = val;
}

خالی ioapic_سیٹ_irq(غیرعلامتی irq)
{
	غیرعلامتی ioa;

	أكد(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	اگر (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		واپس;
	
	أكد(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	برائے (ioa = 0; ioa < nioapics; ioa++) {
		اگر (io_apic[ioa].gsi_بنیاد <= irq &&
				io_apic[ioa].gsi_بنیاد +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, کم_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_بنیاد;
			io_apic_irq[irq].eoi = سیٹ_eoi_طریقہ(irq);
			io_apic_irq[irq].سمتیہ = LAPIC_سمتیہ(irq);

			سیٹ_irq_redir_کم(irq, &کم_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_اندراج_write((خالی *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, کم_32);
		}
	}
}

خالی ioapic_unset_irq(غیرعلامتی irq)
{
	أكد(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = تہی;
	io_apic_irq[irq].eoi = تہی;
}

خالی ioapic_reset_pic(خالی)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

ساکن خالی irq_lapic_حالت(صحیح irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	صحیح سمتیہ;
	ساخت irq * intr;

	intr = &io_apic_irq[irq];
	
	اگر (!intr->ioa)
		واپس;

	سمتیہ = LAPIC_سمتیہ(irq);
	tmr =  apic_read_tmr_سمتیہ(سمتیہ);
	irr =  apic_read_irr_سمتیہ(سمتیہ);
	isr =  apic_read_isr_سمتیہ(سمتیہ);


	اگر (lapic_test_delivery_val(isr, سمتیہ)) {
		لکھو("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, سمتیہ,
				lapic_test_delivery_val(tmr, سمتیہ) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, سمتیہ) ? 1 : 0,
				lapic_test_delivery_val(isr, سمتیہ) ? 1 : 0);
	} ورنہ {
		لکھو("IRQ %d vec %d irr %d\n",
				irq, سمتیہ,
				lapic_test_delivery_val(irr, سمتیہ) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_جدول + intr->pin * 2);
	لکھو("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->سمتیہ,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

خالی dump_apic_irq_state(خالی)
{
	صحیح irq;

	لکھو("--- IRQs state dump ---\n");
	برائے (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_حالت(irq);
	}
	لکھو("--- all ---\n");
}
