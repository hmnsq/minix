/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#అన్తర్భావ <assert.h>

#అన్తర్భావ <unistd.h>
#అన్తర్భావ <minix/portio.h>

#అన్తర్భావ <minix/syslib.h>
#అన్తర్భావ <machine/cmos.h>

#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "apic.h"
#అన్తర్భావ "apic_asm.h"
#అన్తర్భావ "kernel/clock.h"
#అన్తర్భావ "glo.h"
#అన్తర్భావ "hw_intr.h"

#అన్తర్భావ "acpi.h"

#నిర్వచితమైతే USE_WATCHDOG
#అన్తర్భావ "kernel/watchdog.h"
#అయితే_ముగింపు

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

#నిర్వచించు IS_సమితి(ముసుగు)		(ముసుగు)
#నిర్వచించు IS_CLEAR(ముసుగు)		0

#నిర్వచించు APIC_LVTT_సదిశ_ముసుగు	0x000000FF
#నిర్వచించు APIC_LVTT_DS_PENDING	(1 << 12)
#నిర్వచించు APIC_LVTT_ముసుగు		(1 << 16)
#నిర్వచించు APIC_LVTT_TM		(1 << 17)

#నిర్వచించు APIC_LVT_IIPP_ముసుగు	0x00002000
#నిర్వచించు APIC_LVT_IIPP_AH	0x00002000
#నిర్వచించు APIC_LVT_IIPP_AL	0x00000000

#నిర్వచించు APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#నిర్వచించు APIC_LVT_TM_PERIODIC	IS_సమితి(APIC_LVTT_TM)

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
#నిర్వచించు APIC_ICR_INTPOL_దిగువ		IS_సమితి(APIC_ICR_INT_POLARITY)
#నిర్వచించు APIC_ICR_INTPOL_ఎగువ		IS_CLEAR(APIC_ICR_INT_POLARITY)

#నిర్వచించు APIC_ICR_LEVEL_ASSERT		(1 << 14)
#నిర్వచించు APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#నిర్వచించు APIC_ICR_TRIGGER		(1 << 15)
#నిర్వచించు APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#నిర్వచించు APIC_ICR_TM_అంచు		IS_CLEAR(APIC_ICR_TRIGGER)

#నిర్వచించు APIC_ICR_INT_ముసుగు		(1 << 16)

#నిర్వచించు APIC_ICR_DEST_క్షేత్రం		(0 << 18)
#నిర్వచించు APIC_ICR_DEST_SELF		(1 << 18)
#నిర్వచించు APIC_ICR_DEST_ALL		(2 << 18)
#నిర్వచించు APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#నిర్వచించు IA32_APIC_పునాది	0x1b
#నిర్వచించు IA32_APIC_పునాది_ENABLE_బిట్టు	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#నిర్వచించు LAPIC_సదిశ(irq)	(IRQ0_సదిశ +(irq))

#నిర్వచించు IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#నిర్వచించు SPL0				0x0
#నిర్వచించు	SPLHI				0xF


నిర్మాణం io_apic io_apic[MAX_NR_IOAPICS];
గుర్తులేని nioapics;

నిర్మాణం irq;
రకనిర్వచనం శూన్య (* eoi_పద్ధతి_t)(నిర్మాణం irq *);

నిర్మాణం irq {
	నిర్మాణం io_apic * 	ioa;
	గుర్తులేని		pin;
	గుర్తులేని		సదిశ;
	eoi_పద్ధతి_t		eoi;
	గుర్తులేని		state;
};

స్థిర నిర్మాణం irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#నిర్వచితమైతే CONFIG_SMP

#అన్తర్భావ "kernel/smp.h"

#అయితే_ముగింపు

#అన్తర్భావ "kernel/spinlock.h"


#నిర్వచించు lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#నిర్వచించు lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#నిర్వచించు lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#నిర్వచించు lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#నిర్వచించు is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#నిర్వచించు VERBOSE_APIC(x) x

పూర్ణాంకం ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_బైట్లు lapic_addr;
vir_బైట్లు lapic_eoi_addr;
పూర్ణాంకం bsp_lapic_id;

స్థిర అస్థిర గుర్తులేని probe_ticks;
స్థిర	u64_t tsc0, tsc1;
స్థిర	u32_t lapic_tctr0, lapic_tctr1;

స్థిర గుర్తులేని apic_imcrp;
స్థిర స్థిరరాశి గుర్తులేని nlints = 0;

శూన్య arch_eoi(శూన్య)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
స్థిర u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#నిర్వచించు PROBE_TICKS	(system_hz / 10)

#నిర్వచించు IOAPIC_IOREGSEL	0x0
#నిర్వచించు IOAPIC_IOWIN	0x10

స్థిర u32_t ioapic_read(u32_t ioa_పునాది, u32_t reg)
{
	*((అస్థిర u32_t *)(ioa_పునాది + IOAPIC_IOREGSEL)) = (reg & 0xff);
	తిరిగి *(అస్థిర u32_t *)(ioa_పునాది + IOAPIC_IOWIN);
}

స్థిర శూన్య ioapic_write(u32_t ioa_పునాది, u8_t reg, u32_t val)
{
	*((అస్థిర u32_t *)(ioa_పునాది + IOAPIC_IOREGSEL)) = reg;
	*((అస్థిర u32_t *)(ioa_పునాది + IOAPIC_IOWIN)) = val;
}

శూన్య lapic_microsec_sleep(గుర్తులేని లెక్క);
శూన్య apic_idt_init(స్థిరరాశి పూర్ణాంకం reset);

స్థిర శూన్య ioapic_enable_pin(vir_బైట్లు ioapic_addr, పూర్ణాంకం pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_పట్టిక + pin * 2);

	lo &= ~APIC_ICR_INT_ముసుగు;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_పట్టిక + pin * 2, lo);
}

స్థిర శూన్య ioapic_disable_pin(vir_బైట్లు ioapic_addr, పూర్ణాంకం pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_పట్టిక + pin * 2);

	lo |= APIC_ICR_INT_ముసుగు;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_పట్టిక + pin * 2, lo);
}

#ఒకవేళ 0
స్థిర శూన్య ioapic_redirt_నమోదు_read(శూన్య * ioapic_addr,
					పూర్ణాంకం నమోదు_2,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_పట్టిక + నమోదు_2 * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_పట్టిక + నమోదు_2 * 2 + 1));

}
#అయితే_ముగింపు

స్థిర శూన్య ioapic_redirt_నమోదు_write(శూన్య * ioapic_addr,
					పూర్ణాంకం నమోదు_2,
					u32_t hi,
					u32_t lo)
{
#ఒకవేళ 0
	VERBOSE_APIC(రాయి("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", నమోదు_2, hi, lo));
#అయితే_ముగింపు
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_పట్టిక + నమోదు_2 * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_పట్టిక + నమోదు_2 * 2), lo);
}

#నిర్వచించు apic_read_tmr_సదిశ(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#నిర్వచించు apic_read_irr_సదిశ(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#నిర్వచించు apic_read_isr_సదిశ(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#నిర్వచించు lapic_test_delivery_val(val, సదిశ) ((val) & (1 << ((సదిశ) & 0x1f)))

స్థిర శూన్య ioapic_eoi_level(నిర్మాణం irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_సదిశ(irq->సదిశ);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	ఒకవేళ (!lapic_test_delivery_val(tmr, irq->సదిశ)) {
		పూర్ణాంకం is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_పట్టిక + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_ముసుగు;

		/* set mask and edge */
		lo |= APIC_ICR_INT_ముసుగు;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_పట్టిక + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_పట్టిక + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		ఒకవేళ (is_masked)
			lo |= APIC_ICR_INT_ముసుగు;
		లేకపోతే
			lo &= ~APIC_ICR_INT_ముసుగు;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_పట్టిక + irq->pin * 2, lo);
	}
}

స్థిర శూన్య ioapic_eoi_అంచు(__unused నిర్మాణం irq * irq)
{
	apic_eoi();
}

శూన్య ioapic_eoi(పూర్ణాంకం irq)
{
	ఒకవేళ (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	లేకపోతే
		irq_8259_eoi(irq);
}
 
శూన్య ioapic_సమితి_id(u32_t addr, గుర్తులేని పూర్ణాంకం id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

పూర్ణాంకం ioapic_enable_all(శూన్య)
{
	i8259_disable();

	ఒకవేళ (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	తిరిగి ioapic_enabled = 1;
}

/* disables a single IO APIC */
స్థిర శూన్య ioapic_disable(నిర్మాణం io_apic * ioapic)
{
	గుర్తులేని p;
	
	కొరకు (p = 0; p < io_apic->pins; p++) {
		u32_t దిగువ_32, hi_32;
		దిగువ_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_పట్టిక + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_పట్టిక + p * 2 + 1));

		ఒకవేళ (!(దిగువ_32 & APIC_ICR_INT_ముసుగు)) {
			దిగువ_32 |= APIC_ICR_INT_ముసుగు;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_పట్టిక + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_పట్టిక + p * 2), దిగువ_32);
		}
	}
}

/* disables all IO APICs */
శూన్య ioapic_disable_all(శూన్య)
{
	గుర్తులేని ioa;
	ఒకవేళ (!ioapic_enabled)
		తిరిగి;

	కొరకు (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	ఒకవేళ (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

స్థిర శూన్య ioapic_disable_irq(గుర్తులేని irq)
{
	ఒకవేళ(!(io_apic_irq[irq].ioa)) {
		రాయి("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		తిరిగి;
	}

	ప్రతిజ్ఞా(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

స్థిర శూన్య ioapic_enable_irq(గుర్తులేని irq)
{
	ఒకవేళ(!(io_apic_irq[irq].ioa)) {
		రాయి("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		తిరిగి;
	}

	ప్రతిజ్ఞా(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

శూన్య ioapic_unmask_irq(గుర్తులేని irq)
{
	ఒకవేళ (ioapic_enabled)
		ioapic_enable_irq(irq);
	లేకపోతే
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

శూన్య ioapic_ముసుగు_irq(గుర్తులేని irq)
{
	ఒకవేళ (ioapic_enabled)
		ioapic_disable_irq(irq);
	లేకపోతే
		/* FIXME unlikely */
		irq_8259_ముసుగు(irq);
}

గుర్తులేని పూర్ణాంకం apicid(శూన్య)
{
	తిరిగి lapic_read(LAPIC_ID) >> 24;
}

స్థిర పూర్ణాంకం calib_clk_handler(irq_hook_t * వాడని(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	ఒకవేళ (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	లేకపోతే ఒకవేళ (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	తిరిగి 1;
}

స్థిర పూర్ణాంకం spurious_irq_handler(irq_hook_t * వాడని(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	తిరిగి 1;
}

స్థిర శూన్య apic_calibrate_clocks(గుర్తులేని cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(రాయి("Calibrating clock\n"));
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
	lvtt |= APIC_LVTT_ముసుగు;
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
	అయితే(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_తాళం();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(రాయి("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_సమితి_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (గుర్తులేని దీర్ఘ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

శూన్య lapic_సమితి_timer_one_shot(స్థిరరాశి u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_చొప్పున_us;
	స్థిరరాశి u8_t cpu = cpuid;

	ticks_చొప్పున_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_చొప్పున_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_సదిశ;
	lapic_write(LAPIC_LVTTR, lvtt);
}

శూన్య lapic_సమితి_timer_periodic(స్థిరరాశి గుర్తులేని freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_చొప్పున_clock_tick;
	స్థిరరాశి u8_t cpu = cpuid;

	lapic_ticks_చొప్పున_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_సదిశ;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_చొప్పున_clock_tick);
}

శూన్య lapic_stop_timer(శూన్య)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_ముసుగు);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

శూన్య lapic_restart_timer(శూన్య)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	ఒకవేళ (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_సమితి_timer_one_shot(1000000/system_hz);
}

శూన్య lapic_microsec_sleep(గుర్తులేని లెక్క)
{
	lapic_సమితి_timer_one_shot(లెక్క);
	అయితే (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

స్థిర  u32_t lapic_errstatus(శూన్య)
{
	lapic_write(LAPIC_ESR, 0);
	తిరిగి lapic_read(LAPIC_ESR);
}

#నిర్వచితమైతే CONFIG_SMP
స్థిర పూర్ణాంకం lapic_disable_లోపల_msr(శూన్య)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_పునాది, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_పునాది_ENABLE_బిట్టు);
	ia32_msr_write(IA32_APIC_పునాది, msr_hi, msr_lo);

	తిరిగి 1;
}
#అయితే_ముగింపు /* CONFIG_SMP */

శూన్య lapic_disable(శూన్య)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	ఒకవేళ (!lapic_addr)
		తిరిగి;
	
#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ (cpu_is_bsp(cpuid) && !apic_imcrp)
#అయితే_ముగింపు
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_ముసుగు|APIC_ICR_INT_ముసుగు);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		తిరిగి;
	}

#నిర్వచితమైతే CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ముసుగు;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ముసుగు;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_లోపల_msr();
#అయితే_ముగింపు /* CONFIG_SMP */
}

స్థిర పూర్ణాంకం lapic_enable_లోపల_msr(శూన్య)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_పునాది, &msr_hi, &msr_lo);

#ఒకవేళ 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	ఒకవేళ (addr != (lapic_addr >> 12)) {
		ఒకవేళ (msr_hi & 0xf) {
			రాయి("ERROR : APIC address needs more then 32 bits\n");
			తిరిగి 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#అయితే_ముగింపు

	msr_lo |= (1 << IA32_APIC_పునాది_ENABLE_బిట్టు);
	ia32_msr_write(IA32_APIC_పునాది, msr_hi, msr_lo);

	తిరిగి 1;
}

పూర్ణాంకం lapic_enable(గుర్తులేని cpu)
{
	u32_t val, nlvt;

	ఒకవేళ (!lapic_addr)
		తిరిగి 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	ఒకవేళ (!cpu_has_tsc) {
		రాయి("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		తిరిగి 0;
	}

	ఒకవేళ (!lapic_enable_లోపల_msr())
		తిరిగి 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_సదిశ;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(శూన్య) lapic_read(LAPIC_SIVR);

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

	nlvt = (lapic_read(LAPIC_సంస్కరణ)>>16) & 0xFF;

	ఒకవేళ(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_ముసుగు);
	}

	ఒకవేళ(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_ముసుగు);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(శూన్య) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(రాయి("APIC timer calibrated\n"));

	తిరిగి 1;
}

శూన్య apic_spurios_intr_handler(శూన్య)
{
	స్థిర గుర్తులేని x;

	x++;
	ఒకవేళ (x == 1 || (x % 100) == 0)
		రాయి("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

శూన్య apic_error_intr_handler(శూన్య)
{
	స్థిర గుర్తులేని x;

	x++;
	ఒకవేళ (x == 1 || (x % 100) == 0)
		రాయి("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

స్థిర నిర్మాణం gate_పట్టిక_s gate_పట్టిక_ioapic[] = {
	{ apic_hwint0, LAPIC_సదిశ( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_సదిశ( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_సదిశ( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_సదిశ( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_సదిశ( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_సదిశ( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_సదిశ( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_సదిశ( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_సదిశ( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_సదిశ( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_సదిశ(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_సదిశ(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_సదిశ(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_సదిశ(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_సదిశ(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_సదిశ(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_సదిశ(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_సదిశ(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_సదిశ(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_సదిశ(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_సదిశ(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_సదిశ(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_సదిశ(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_సదిశ(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_సదిశ(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_సదిశ(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_సదిశ(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_సదిశ(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_సదిశ(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_సదిశ(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_సదిశ(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_సదిశ(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_సదిశ(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_సదిశ(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_సదిశ(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_సదిశ(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_సదిశ(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_సదిశ(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_సదిశ(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_సదిశ(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_సదిశ(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_సదిశ(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_సదిశ(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_సదిశ(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_సదిశ(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_సదిశ(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_సదిశ(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_సదిశ(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_సదిశ(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_సదిశ(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_సదిశ(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_సదిశ(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_సదిశ(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_సదిశ(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_సదిశ(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_సదిశ(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_సదిశ(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_సదిశ(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_సదిశ(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_సదిశ(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_సదిశ(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_సదిశ(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_సదిశ(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_సదిశ(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_సదిశ, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_సదిశ, INTR_PRIVILEGE },
	{ ఖాళీ, 0, 0}
};

స్థిర నిర్మాణం gate_పట్టిక_s gate_పట్టిక_common[] = {
	{ ipc_నమోదు_softint_orig, IPC_సదిశ_ORIG, USER_PRIVILEGE },
	{ kernel_call_నమోదు_orig, KERN_CALL_సదిశ_ORIG, USER_PRIVILEGE },
	{ ipc_నమోదు_softint_um, IPC_సదిశ_UM, USER_PRIVILEGE },
	{ kernel_call_నమోదు_um, KERN_CALL_సదిశ_UM, USER_PRIVILEGE },
	{ ఖాళీ, 0, 0}
};

#నిర్వచితమైతే CONFIG_SMP
స్థిర నిర్మాణం gate_పట్టిక_s gate_పట్టిక_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_సదిశ, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_సదిశ, INTR_PRIVILEGE },
	{ ఖాళీ, 0, 0}
};
#అయితే_ముగింపు

#నిర్వచితమైతే APIC_DEBUG
స్థిర శూన్య lapic_సమితి_dummy_handlers(శూన్య)
{
	అక్షరం * handler;
	పూర్ణాంకం vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_ప్రారంభం;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_కొలత;
	కొరకు(; handler < &lapic_intr_dummy_handles_చివర;
			handler += LAPIC_INTR_DUMMY_HANDLER_కొలత) {
		int_gate_idt(vect++, (vir_బైట్లు) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_జరుపు));
	}
}
#అయితే_ముగింపు

/* Build descriptors for interrupt gates in IDT. */
శూన్య apic_idt_init(స్థిరరాశి పూర్ణాంకం reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	పూర్ణాంకం is_bsp;

	ఒకవేళ (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_పట్టిక_common);
		తిరిగి;
	}

	is_bsp = is_boot_apic(apicid());

#నిర్వచితమైతే APIC_DEBUG
	ఒకవేళ (is_bsp)
		రాయి("APIC debugging is enabled\n");
	lapic_సమితి_dummy_handlers();
#అయితే_ముగింపు

	/* Build descriptors for interrupt gates in IDT. */
	ఒకవేళ (ioapic_enabled)
		idt_copy_vectors(gate_పట్టిక_ioapic);
	లేకపోతే
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_పట్టిక_common);

#నిర్వచితమైతే CONFIG_SMP
	idt_copy_vectors(gate_పట్టిక_smp);
#అయితే_ముగింపు

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_సదిశ;
	val &= ~ APIC_ICR_INT_ముసుగు;
	lapic_write(LAPIC_LVTER, val);
	(శూన్య) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	ఒకవేళ (is_bsp) {
		BOOT_VERBOSE(రాయి("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_సదిశ, (vir_బైట్లు) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_జరుపు));
	}

}

స్థిర పూర్ణాంకం acpi_get_ioapics(నిర్మాణం io_apic * ioa, గుర్తులేని * nioa, గుర్తులేని max)
{
	గుర్తులేని n = 0;
	నిర్మాణం acpi_madt_ioapic * acpi_ioa;

	అయితే (n < max) {
		acpi_ioa = acpi_get_ioapic_తదుపరి();
		ఒకవేళ (acpi_ioa == ఖాళీ)
			ఆపు;

		ప్రతిజ్ఞా(acpi_ioa->చిరునామా);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->చిరునామా;
		ioa[n].paddr = (phys_బైట్లు) acpi_ioa->చిరునామా;
		ioa[n].gsi_పునాది = acpi_ioa->global_int_పునాది;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_సంస్కరణ) & 0xff0000) >> 16)+1;
		రాయి("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	తిరిగి n;
}

పూర్ణాంకం detect_ioapics(శూన్య)
{
	పూర్ణాంకం స్థితి_2;

	ఒకవేళ (machine.acpi_rsdp) {
		స్థితి_2 = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} లేకపోతే {
		స్థితి_2 = 0;
	}
	ఒకవేళ (!స్థితి_2) {
		/* try something different like MPS */
	}

	తిరిగి స్థితి_2;
}

#నిర్వచితమైతే CONFIG_SMP

శూన్య apic_send_ipi(గుర్తులేని సదిశ, గుర్తులేని cpu, పూర్ణాంకం type)
{
	u32_t icr1, icr2;

	ఒకవేళ (ncpus == 1)
		/* no need of sending an IPI */
		తిరిగి;

	అయితే (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	ఎంపిక (type) {
		స్థితి APIC_IPI_DEST:
			ఒకవేళ (!cpu_is_ready(cpu))
				తిరిగి;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_క్షేత్రం | సదిశ);
			ఆపు;
		స్థితి APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | సదిశ);
			ఆపు;
		స్థితి APIC_IPI_గమ్యం_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | సదిశ);
			ఆపు;
		స్థితి APIC_IPI_గమ్యం_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | సదిశ);
			ఆపు;
		అప్రమేయ:
			రాయి("WARNING : unknown send ipi type request\n");
	}

}

పూర్ణాంకం apic_send_startup_ipi(గుర్తులేని cpu, phys_బైట్లు trampoline)
{
	పూర్ణాంకం timeout;
	u32_t errstatus = 0;
	పూర్ణాంకం i;

	/* INIT-SIPI-SIPI sequence */

	కొరకు (i = 0; i < 2; i++) {
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

		అయితే ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			ఒకవేళ (!timeout) ఆపు;
		}

		/* skip this one and continue with another cpu */
		ఒకవేళ (errstatus)
			తిరిగి -1;
	}

	తిరిగి 0;
}

పూర్ణాంకం apic_send_init_ipi(గుర్తులేని cpu, phys_బైట్లు trampoline) 
{
	u32_t ptr, errstatus = 0;
	పూర్ణాంకం timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), పరిమాణం(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), పరిమాణం(u16_t ));

	/* set shutdown code */
	outb (RTC_సూచీ, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(శూన్య) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	అయితే ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ఒకవేళ (!timeout) ఆపు;
	}

	ఒకవేళ (errstatus) 
		తిరిగి -1; /* to continue with a new processor */

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

	అయితే ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ఒకవేళ(!timeout) ఆపు;
	}

	ఒకవేళ (errstatus) 
		తిరిగి -1; /* with the new processor */

	/* clear error state register.  */
	(శూన్య) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	తిరిగి 0;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CONFIG_SMP
పూర్ణాంకం apic_single_cpu_init(శూన్య)
{
	ఒకవేళ (!cpu_feature_apic_on_chip())
		తిరిగి 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	ఒకవేళ (!lapic_enable(0)) {
		lapic_addr = 0x0;
		తిరిగి 0;
	}

	bsp_lapic_id = apicid();
	రాయి("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	ఒకవేళ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		తిరిగి 0;
	}

	ioapic_enable_all();

	ఒకవేళ (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	తిరిగి 1;
}
#అయితే_ముగింపు

స్థిర eoi_పద్ధతి_t సమితి_eoi_పద్ధతి(గుర్తులేని irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	ఒకవేళ (irq < 16)
		తిరిగి ioapic_eoi_అంచు;
	లేకపోతే
		తిరిగి ioapic_eoi_level;
}

శూన్య సమితి_irq_redir_దిగువ(గుర్తులేని irq, u32_t * దిగువ)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_సదిశ | APIC_ICR_INT_ముసుగు |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	ఒకవేళ (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	లేకపోతే {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].సదిశ;

	*దిగువ = val;
}

శూన్య ioapic_సమితి_irq(గుర్తులేని irq)
{
	గుర్తులేని ioa;

	ప్రతిజ్ఞా(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	ఒకవేళ (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		తిరిగి;
	
	ప్రతిజ్ఞా(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	కొరకు (ioa = 0; ioa < nioapics; ioa++) {
		ఒకవేళ (io_apic[ioa].gsi_పునాది <= irq &&
				io_apic[ioa].gsi_పునాది +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, దిగువ_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_పునాది;
			io_apic_irq[irq].eoi = సమితి_eoi_పద్ధతి(irq);
			io_apic_irq[irq].సదిశ = LAPIC_సదిశ(irq);

			సమితి_irq_redir_దిగువ(irq, &దిగువ_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_నమోదు_write((శూన్య *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, దిగువ_32);
		}
	}
}

శూన్య ioapic_unset_irq(గుర్తులేని irq)
{
	ప్రతిజ్ఞా(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = ఖాళీ;
	io_apic_irq[irq].eoi = ఖాళీ;
}

శూన్య ioapic_reset_pic(శూన్య)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

స్థిర శూన్య irq_lapic_స్థితి(పూర్ణాంకం irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	పూర్ణాంకం సదిశ;
	నిర్మాణం irq * intr;

	intr = &io_apic_irq[irq];
	
	ఒకవేళ (!intr->ioa)
		తిరిగి;

	సదిశ = LAPIC_సదిశ(irq);
	tmr =  apic_read_tmr_సదిశ(సదిశ);
	irr =  apic_read_irr_సదిశ(సదిశ);
	isr =  apic_read_isr_సదిశ(సదిశ);


	ఒకవేళ (lapic_test_delivery_val(isr, సదిశ)) {
		రాయి("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, సదిశ,
				lapic_test_delivery_val(tmr, సదిశ) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, సదిశ) ? 1 : 0,
				lapic_test_delivery_val(isr, సదిశ) ? 1 : 0);
	} లేకపోతే {
		రాయి("IRQ %d vec %d irr %d\n",
				irq, సదిశ,
				lapic_test_delivery_val(irr, సదిశ) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_పట్టిక + intr->pin * 2);
	రాయి("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->సదిశ,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

శూన్య dump_apic_irq_state(శూన్య)
{
	పూర్ణాంకం irq;

	రాయి("--- IRQs state dump ---\n");
	కొరకు (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_స్థితి(irq);
	}
	రాయి("--- all ---\n");
}
