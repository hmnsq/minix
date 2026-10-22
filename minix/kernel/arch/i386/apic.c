/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#अन्तर्भाव <assert.h>

#अन्तर्भाव <unistd.h>
#अन्तर्भाव <minix/portio.h>

#अन्तर्भाव <minix/syslib.h>
#अन्तर्भाव <machine/cmos.h>

#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "apic.h"
#अन्तर्भाव "apic_asm.h"
#अन्तर्भाव "kernel/clock.h"
#अन्तर्भाव "glo.h"
#अन्तर्भाव "hw_intr.h"

#अन्तर्भाव "acpi.h"

#यदि_परिभाषितम् USE_WATCHDOG
#अन्तर्भाव "kernel/watchdog.h"
#यदि_अन्तः

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

#परिभाषय IS_समुच्चयः(आवरणम्)		(आवरणम्)
#परिभाषय IS_CLEAR(आवरणम्)		0

#परिभाषय APIC_LVTT_सदिशम्_आवरणम्	0x000000FF
#परिभाषय APIC_LVTT_DS_PENDING	(1 << 12)
#परिभाषय APIC_LVTT_आवरणम्		(1 << 16)
#परिभाषय APIC_LVTT_TM		(1 << 17)

#परिभाषय APIC_LVT_IIPP_आवरणम्	0x00002000
#परिभाषय APIC_LVT_IIPP_AH	0x00002000
#परिभाषय APIC_LVT_IIPP_AL	0x00000000

#परिभाषय APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#परिभाषय APIC_LVT_TM_PERIODIC	IS_समुच्चयः(APIC_LVTT_TM)

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
#परिभाषय APIC_ICR_INTPOL_नीचः		IS_समुच्चयः(APIC_ICR_INT_POLARITY)
#परिभाषय APIC_ICR_INTPOL_उच्चः		IS_CLEAR(APIC_ICR_INT_POLARITY)

#परिभाषय APIC_ICR_LEVEL_ASSERT		(1 << 14)
#परिभाषय APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#परिभाषय APIC_ICR_TRIGGER		(1 << 15)
#परिभाषय APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#परिभाषय APIC_ICR_TM_EDGE		IS_CLEAR(APIC_ICR_TRIGGER)

#परिभाषय APIC_ICR_INT_आवरणम्		(1 << 16)

#परिभाषय APIC_ICR_DEST_क्षेत्रम्		(0 << 18)
#परिभाषय APIC_ICR_DEST_SELF		(1 << 18)
#परिभाषय APIC_ICR_DEST_ALL		(2 << 18)
#परिभाषय APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#परिभाषय IA32_APIC_आधारः	0x1b
#परिभाषय IA32_APIC_आधारः_ENABLE_BIT	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#परिभाषय LAPIC_सदिशम्(irq)	(IRQ0_सदिशम् +(irq))

#परिभाषय IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#परिभाषय SPL0				0x0
#परिभाषय	SPLHI				0xF


संरचना io_apic io_apic[MAX_NR_IOAPICS];
अचिह्नित nioapics;

संरचना irq;
प्रकारनिर्देश शून्य (* eoi_विधानम्_t)(संरचना irq *);

संरचना irq {
	संरचना io_apic * 	ioa;
	अचिह्नित		pin;
	अचिह्नित		सदिशम्;
	eoi_विधानम्_t		eoi;
	अचिह्नित		state;
};

स्थिर संरचना irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#यदि_परिभाषितम् CONFIG_SMP

#अन्तर्भाव "kernel/smp.h"

#यदि_अन्तः

#अन्तर्भाव "kernel/spinlock.h"


#परिभाषय lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#परिभाषय lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#परिभाषय lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#परिभाषय lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#परिभाषय is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#परिभाषय VERBOSE_APIC(x) x

पूर्णाङ्क ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_अष्टकानि lapic_addr;
vir_अष्टकानि lapic_eoi_addr;
पूर्णाङ्क bsp_lapic_id;

स्थिर चञ्चल अचिह्नित probe_ticks;
स्थिर	u64_t tsc0, tsc1;
स्थिर	u32_t lapic_tctr0, lapic_tctr1;

स्थिर अचिह्नित apic_imcrp;
स्थिर ध्रुव अचिह्नित nlints = 0;

शून्य arch_eoi(शून्य)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
स्थिर u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#परिभाषय PROBE_TICKS	(system_hz / 10)

#परिभाषय IOAPIC_IOREGSEL	0x0
#परिभाषय IOAPIC_IOWIN	0x10

स्थिर u32_t ioapic_read(u32_t ioa_आधारः, u32_t reg)
{
	*((चञ्चल u32_t *)(ioa_आधारः + IOAPIC_IOREGSEL)) = (reg & 0xff);
	प्रत्यागम *(चञ्चल u32_t *)(ioa_आधारः + IOAPIC_IOWIN);
}

स्थिर शून्य ioapic_write(u32_t ioa_आधारः, u8_t reg, u32_t val)
{
	*((चञ्चल u32_t *)(ioa_आधारः + IOAPIC_IOREGSEL)) = reg;
	*((चञ्चल u32_t *)(ioa_आधारः + IOAPIC_IOWIN)) = val;
}

शून्य lapic_microsec_sleep(अचिह्नित गणनम्);
शून्य apic_idt_init(ध्रुव पूर्णाङ्क reset);

स्थिर शून्य ioapic_enable_pin(vir_अष्टकानि ioapic_addr, पूर्णाङ्क pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_सारणी + pin * 2);

	lo &= ~APIC_ICR_INT_आवरणम्;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_सारणी + pin * 2, lo);
}

स्थिर शून्य ioapic_disable_pin(vir_अष्टकानि ioapic_addr, पूर्णाङ्क pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_सारणी + pin * 2);

	lo |= APIC_ICR_INT_आवरणम्;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_सारणी + pin * 2, lo);
}

#यदि 0
स्थिर शून्य ioapic_redirt_प्रविष्टिः_read(शून्य * ioapic_addr,
					पूर्णाङ्क प्रविष्टिः,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_सारणी + प्रविष्टिः * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_सारणी + प्रविष्टिः * 2 + 1));

}
#यदि_अन्तः

स्थिर शून्य ioapic_redirt_प्रविष्टिः_write(शून्य * ioapic_addr,
					पूर्णाङ्क प्रविष्टिः,
					u32_t hi,
					u32_t lo)
{
#यदि 0
	VERBOSE_APIC(प्रारूप_लिख("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", प्रविष्टिः, hi, lo));
#यदि_अन्तः
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_सारणी + प्रविष्टिः * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_सारणी + प्रविष्टिः * 2), lo);
}

#परिभाषय apic_read_tmr_सदिशम्(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#परिभाषय apic_read_irr_सदिशम्(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#परिभाषय apic_read_isr_सदिशम्(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#परिभाषय lapic_test_delivery_val(val, सदिशम्) ((val) & (1 << ((सदिशम्) & 0x1f)))

स्थिर शून्य ioapic_eoi_level(संरचना irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_सदिशम्(irq->सदिशम्);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	यदि (!lapic_test_delivery_val(tmr, irq->सदिशम्)) {
		पूर्णाङ्क is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_सारणी + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_आवरणम्;

		/* set mask and edge */
		lo |= APIC_ICR_INT_आवरणम्;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_सारणी + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_सारणी + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		यदि (is_masked)
			lo |= APIC_ICR_INT_आवरणम्;
		अन्यथा
			lo &= ~APIC_ICR_INT_आवरणम्;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_सारणी + irq->pin * 2, lo);
	}
}

स्थिर शून्य ioapic_eoi_edge(__unused संरचना irq * irq)
{
	apic_eoi();
}

शून्य ioapic_eoi(पूर्णाङ्क irq)
{
	यदि (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	अन्यथा
		irq_8259_eoi(irq);
}
 
शून्य ioapic_समुच्चयः_id(u32_t addr, अचिह्नित पूर्णाङ्क id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

पूर्णाङ्क ioapic_enable_all(शून्य)
{
	i8259_disable();

	यदि (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	प्रत्यागम ioapic_enabled = 1;
}

/* disables a single IO APIC */
स्थिर शून्य ioapic_disable(संरचना io_apic * ioapic)
{
	अचिह्नित p;
	
	प्रति (p = 0; p < io_apic->pins; p++) {
		u32_t नीचः_32, hi_32;
		नीचः_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_सारणी + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_सारणी + p * 2 + 1));

		यदि (!(नीचः_32 & APIC_ICR_INT_आवरणम्)) {
			नीचः_32 |= APIC_ICR_INT_आवरणम्;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_सारणी + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_सारणी + p * 2), नीचः_32);
		}
	}
}

/* disables all IO APICs */
शून्य ioapic_disable_all(शून्य)
{
	अचिह्नित ioa;
	यदि (!ioapic_enabled)
		प्रत्यागम;

	प्रति (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	यदि (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

स्थिर शून्य ioapic_disable_irq(अचिह्नित irq)
{
	यदि(!(io_apic_irq[irq].ioa)) {
		प्रारूप_लिख("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		प्रत्यागम;
	}

	प्रतिज्ञा(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

स्थिर शून्य ioapic_enable_irq(अचिह्नित irq)
{
	यदि(!(io_apic_irq[irq].ioa)) {
		प्रारूप_लिख("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		प्रत्यागम;
	}

	प्रतिज्ञा(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

शून्य ioapic_unmask_irq(अचिह्नित irq)
{
	यदि (ioapic_enabled)
		ioapic_enable_irq(irq);
	अन्यथा
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

शून्य ioapic_आवरणम्_irq(अचिह्नित irq)
{
	यदि (ioapic_enabled)
		ioapic_disable_irq(irq);
	अन्यथा
		/* FIXME unlikely */
		irq_8259_आवरणम्(irq);
}

अचिह्नित पूर्णाङ्क apicid(शून्य)
{
	प्रत्यागम lapic_read(LAPIC_ID) >> 24;
}

स्थिर पूर्णाङ्क calib_clk_handler(irq_hook_t * अप्रयुक्तम्(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	यदि (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	अन्यथा यदि (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	प्रत्यागम 1;
}

स्थिर पूर्णाङ्क spurious_irq_handler(irq_hook_t * अप्रयुक्तम्(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	प्रत्यागम 1;
}

स्थिर शून्य apic_calibrate_clocks(अचिह्नित cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(प्रारूप_लिख("Calibrating clock\n"));
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
	lvtt |= APIC_LVTT_आवरणम्;
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
	यावत्(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_तालकम्();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(प्रारूप_लिख("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_समुच्चयः_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (अचिह्नित दीर्घ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

शून्य lapic_समुच्चयः_timer_one_shot(ध्रुव u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_प्रत्येकम्_us;
	ध्रुव u8_t cpu = cpuid;

	ticks_प्रत्येकम्_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_प्रत्येकम्_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_सदिशम्;
	lapic_write(LAPIC_LVTTR, lvtt);
}

शून्य lapic_समुच्चयः_timer_periodic(ध्रुव अचिह्नित freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_प्रत्येकम्_clock_tick;
	ध्रुव u8_t cpu = cpuid;

	lapic_ticks_प्रत्येकम्_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_सदिशम्;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_प्रत्येकम्_clock_tick);
}

शून्य lapic_stop_timer(शून्य)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_आवरणम्);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

शून्य lapic_restart_timer(शून्य)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	यदि (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_समुच्चयः_timer_one_shot(1000000/system_hz);
}

शून्य lapic_microsec_sleep(अचिह्नित गणनम्)
{
	lapic_समुच्चयः_timer_one_shot(गणनम्);
	यावत् (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

स्थिर  u32_t lapic_errstatus(शून्य)
{
	lapic_write(LAPIC_ESR, 0);
	प्रत्यागम lapic_read(LAPIC_ESR);
}

#यदि_परिभाषितम् CONFIG_SMP
स्थिर पूर्णाङ्क lapic_disable_अन्तर्_msr(शून्य)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_आधारः, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_आधारः_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_आधारः, msr_hi, msr_lo);

	प्रत्यागम 1;
}
#यदि_अन्तः /* CONFIG_SMP */

शून्य lapic_disable(शून्य)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	यदि (!lapic_addr)
		प्रत्यागम;
	
#यदि_परिभाषितम् CONFIG_SMP
	यदि (cpu_is_bsp(cpuid) && !apic_imcrp)
#यदि_अन्तः
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_आवरणम्|APIC_ICR_INT_आवरणम्);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		प्रत्यागम;
	}

#यदि_परिभाषितम् CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_आवरणम्;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_आवरणम्;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_अन्तर्_msr();
#यदि_अन्तः /* CONFIG_SMP */
}

स्थिर पूर्णाङ्क lapic_enable_अन्तर्_msr(शून्य)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_आधारः, &msr_hi, &msr_lo);

#यदि 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	यदि (addr != (lapic_addr >> 12)) {
		यदि (msr_hi & 0xf) {
			प्रारूप_लिख("ERROR : APIC address needs more then 32 bits\n");
			प्रत्यागम 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#यदि_अन्तः

	msr_lo |= (1 << IA32_APIC_आधारः_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_आधारः, msr_hi, msr_lo);

	प्रत्यागम 1;
}

पूर्णाङ्क lapic_enable(अचिह्नित cpu)
{
	u32_t val, nlvt;

	यदि (!lapic_addr)
		प्रत्यागम 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	यदि (!cpu_has_tsc) {
		प्रारूप_लिख("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		प्रत्यागम 0;
	}

	यदि (!lapic_enable_अन्तर्_msr())
		प्रत्यागम 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_सदिशम्;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(शून्य) lapic_read(LAPIC_SIVR);

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

	nlvt = (lapic_read(LAPIC_संस्करणम्)>>16) & 0xFF;

	यदि(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_आवरणम्);
	}

	यदि(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_आवरणम्);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(शून्य) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(प्रारूप_लिख("APIC timer calibrated\n"));

	प्रत्यागम 1;
}

शून्य apic_spurios_intr_handler(शून्य)
{
	स्थिर अचिह्नित x;

	x++;
	यदि (x == 1 || (x % 100) == 0)
		प्रारूप_लिख("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

शून्य apic_error_intr_handler(शून्य)
{
	स्थिर अचिह्नित x;

	x++;
	यदि (x == 1 || (x % 100) == 0)
		प्रारूप_लिख("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

स्थिर संरचना gate_सारणी_s gate_सारणी_ioapic[] = {
	{ apic_hwint0, LAPIC_सदिशम्( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_सदिशम्( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_सदिशम्( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_सदिशम्( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_सदिशम्( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_सदिशम्( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_सदिशम्( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_सदिशम्( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_सदिशम्( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_सदिशम्( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_सदिशम्(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_सदिशम्(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_सदिशम्(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_सदिशम्(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_सदिशम्(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_सदिशम्(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_सदिशम्(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_सदिशम्(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_सदिशम्(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_सदिशम्(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_सदिशम्(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_सदिशम्(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_सदिशम्(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_सदिशम्(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_सदिशम्(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_सदिशम्(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_सदिशम्(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_सदिशम्(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_सदिशम्(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_सदिशम्(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_सदिशम्(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_सदिशम्(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_सदिशम्(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_सदिशम्(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_सदिशम्(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_सदिशम्(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_सदिशम्(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_सदिशम्(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_सदिशम्(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_सदिशम्(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_सदिशम्(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_सदिशम्(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_सदिशम्(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_सदिशम्(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_सदिशम्(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_सदिशम्(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_सदिशम्(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_सदिशम्(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_सदिशम्(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_सदिशम्(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_सदिशम्(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_सदिशम्(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_सदिशम्(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_सदिशम्(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_सदिशम्(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_सदिशम्(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_सदिशम्(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_सदिशम्(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_सदिशम्(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_सदिशम्(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_सदिशम्(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_सदिशम्(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_सदिशम्(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_सदिशम्(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_सदिशम्, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_सदिशम्, INTR_PRIVILEGE },
	{ रिक्त, 0, 0}
};

स्थिर संरचना gate_सारणी_s gate_सारणी_common[] = {
	{ ipc_प्रविष्टिः_softint_orig, IPC_सदिशम्_ORIG, USER_PRIVILEGE },
	{ kernel_call_प्रविष्टिः_orig, KERN_CALL_सदिशम्_ORIG, USER_PRIVILEGE },
	{ ipc_प्रविष्टिः_softint_um, IPC_सदिशम्_UM, USER_PRIVILEGE },
	{ kernel_call_प्रविष्टिः_um, KERN_CALL_सदिशम्_UM, USER_PRIVILEGE },
	{ रिक्त, 0, 0}
};

#यदि_परिभाषितम् CONFIG_SMP
स्थिर संरचना gate_सारणी_s gate_सारणी_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_सदिशम्, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_सदिशम्, INTR_PRIVILEGE },
	{ रिक्त, 0, 0}
};
#यदि_अन्तः

#यदि_परिभाषितम् APIC_DEBUG
स्थिर शून्य lapic_समुच्चयः_dummy_handlers(शून्य)
{
	वर्ण * handler;
	पूर्णाङ्क vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_start;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_आकार;
	प्रति(; handler < &lapic_intr_dummy_handles_अन्तः;
			handler += LAPIC_INTR_DUMMY_HANDLER_आकार) {
		int_gate_idt(vect++, (vir_अष्टकानि) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_सरणम्));
	}
}
#यदि_अन्तः

/* Build descriptors for interrupt gates in IDT. */
शून्य apic_idt_init(ध्रुव पूर्णाङ्क reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	पूर्णाङ्क is_bsp;

	यदि (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_सारणी_common);
		प्रत्यागम;
	}

	is_bsp = is_boot_apic(apicid());

#यदि_परिभाषितम् APIC_DEBUG
	यदि (is_bsp)
		प्रारूप_लिख("APIC debugging is enabled\n");
	lapic_समुच्चयः_dummy_handlers();
#यदि_अन्तः

	/* Build descriptors for interrupt gates in IDT. */
	यदि (ioapic_enabled)
		idt_copy_vectors(gate_सारणी_ioapic);
	अन्यथा
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_सारणी_common);

#यदि_परिभाषितम् CONFIG_SMP
	idt_copy_vectors(gate_सारणी_smp);
#यदि_अन्तः

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_सदिशम्;
	val &= ~ APIC_ICR_INT_आवरणम्;
	lapic_write(LAPIC_LVTER, val);
	(शून्य) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	यदि (is_bsp) {
		BOOT_VERBOSE(प्रारूप_लिख("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_सदिशम्, (vir_अष्टकानि) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_सरणम्));
	}

}

स्थिर पूर्णाङ्क acpi_get_ioapics(संरचना io_apic * ioa, अचिह्नित * nioa, अचिह्नित max)
{
	अचिह्नित n = 0;
	संरचना acpi_madt_ioapic * acpi_ioa;

	यावत् (n < max) {
		acpi_ioa = acpi_get_ioapic_अग्र();
		यदि (acpi_ioa == रिक्त)
			भङ्ग;

		प्रतिज्ञा(acpi_ioa->स्थानाङ्कः);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->स्थानाङ्कः;
		ioa[n].paddr = (phys_अष्टकानि) acpi_ioa->स्थानाङ्कः;
		ioa[n].gsi_आधारः = acpi_ioa->global_int_आधारः;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_संस्करणम्) & 0xff0000) >> 16)+1;
		प्रारूप_लिख("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	प्रत्यागम n;
}

पूर्णाङ्क detect_ioapics(शून्य)
{
	पूर्णाङ्क स्थितिः;

	यदि (machine.acpi_rsdp) {
		स्थितिः = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} अन्यथा {
		स्थितिः = 0;
	}
	यदि (!स्थितिः) {
		/* try something different like MPS */
	}

	प्रत्यागम स्थितिः;
}

#यदि_परिभाषितम् CONFIG_SMP

शून्य apic_send_ipi(अचिह्नित सदिशम्, अचिह्नित cpu, पूर्णाङ्क type)
{
	u32_t icr1, icr2;

	यदि (ncpus == 1)
		/* no need of sending an IPI */
		प्रत्यागम;

	यावत् (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	विकल्प (type) {
		पक्ष APIC_IPI_DEST:
			यदि (!cpu_is_ready(cpu))
				प्रत्यागम;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_क्षेत्रम् | सदिशम्);
			भङ्ग;
		पक्ष APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | सदिशम्);
			भङ्ग;
		पक्ष APIC_IPI_गन्तव्यम्_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | सदिशम्);
			भङ्ग;
		पक्ष APIC_IPI_गन्तव्यम्_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | सदिशम्);
			भङ्ग;
		स्वभाव:
			प्रारूप_लिख("WARNING : unknown send ipi type request\n");
	}

}

पूर्णाङ्क apic_send_startup_ipi(अचिह्नित cpu, phys_अष्टकानि trampoline)
{
	पूर्णाङ्क timeout;
	u32_t errstatus = 0;
	पूर्णाङ्क i;

	/* INIT-SIPI-SIPI sequence */

	प्रति (i = 0; i < 2; i++) {
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

		यावत् ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			यदि (!timeout) भङ्ग;
		}

		/* skip this one and continue with another cpu */
		यदि (errstatus)
			प्रत्यागम -1;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क apic_send_init_ipi(अचिह्नित cpu, phys_अष्टकानि trampoline) 
{
	u32_t ptr, errstatus = 0;
	पूर्णाङ्क timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), परिमाण(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), परिमाण(u16_t ));

	/* set shutdown code */
	outb (RTC_सूचकांक, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(शून्य) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	यावत् ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		यदि (!timeout) भङ्ग;
	}

	यदि (errstatus) 
		प्रत्यागम -1; /* to continue with a new processor */

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

	यावत् ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		यदि(!timeout) भङ्ग;
	}

	यदि (errstatus) 
		प्रत्यागम -1; /* with the new processor */

	/* clear error state register.  */
	(शून्य) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	प्रत्यागम 0;
}
#यदि_अन्तः

#यदि_अपरिभाषितम् CONFIG_SMP
पूर्णाङ्क apic_single_cpu_init(शून्य)
{
	यदि (!cpu_feature_apic_on_chip())
		प्रत्यागम 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	यदि (!lapic_enable(0)) {
		lapic_addr = 0x0;
		प्रत्यागम 0;
	}

	bsp_lapic_id = apicid();
	प्रारूप_लिख("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	यदि (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		प्रत्यागम 0;
	}

	ioapic_enable_all();

	यदि (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	प्रत्यागम 1;
}
#यदि_अन्तः

स्थिर eoi_विधानम्_t समुच्चयः_eoi_विधानम्(अचिह्नित irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	यदि (irq < 16)
		प्रत्यागम ioapic_eoi_edge;
	अन्यथा
		प्रत्यागम ioapic_eoi_level;
}

शून्य समुच्चयः_irq_redir_नीचः(अचिह्नित irq, u32_t * नीचः)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_सदिशम् | APIC_ICR_INT_आवरणम् |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	यदि (irq < 16) {
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

	val |= io_apic_irq[irq].सदिशम्;

	*नीचः = val;
}

शून्य ioapic_समुच्चयः_irq(अचिह्नित irq)
{
	अचिह्नित ioa;

	प्रतिज्ञा(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	यदि (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		प्रत्यागम;
	
	प्रतिज्ञा(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	प्रति (ioa = 0; ioa < nioapics; ioa++) {
		यदि (io_apic[ioa].gsi_आधारः <= irq &&
				io_apic[ioa].gsi_आधारः +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, नीचः_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_आधारः;
			io_apic_irq[irq].eoi = समुच्चयः_eoi_विधानम्(irq);
			io_apic_irq[irq].सदिशम् = LAPIC_सदिशम्(irq);

			समुच्चयः_irq_redir_नीचः(irq, &नीचः_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_प्रविष्टिः_write((शून्य *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, नीचः_32);
		}
	}
}

शून्य ioapic_unset_irq(अचिह्नित irq)
{
	प्रतिज्ञा(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = रिक्त;
	io_apic_irq[irq].eoi = रिक्त;
}

शून्य ioapic_reset_pic(शून्य)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

स्थिर शून्य irq_lapic_स्थितिः(पूर्णाङ्क irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	पूर्णाङ्क सदिशम्;
	संरचना irq * intr;

	intr = &io_apic_irq[irq];
	
	यदि (!intr->ioa)
		प्रत्यागम;

	सदिशम् = LAPIC_सदिशम्(irq);
	tmr =  apic_read_tmr_सदिशम्(सदिशम्);
	irr =  apic_read_irr_सदिशम्(सदिशम्);
	isr =  apic_read_isr_सदिशम्(सदिशम्);


	यदि (lapic_test_delivery_val(isr, सदिशम्)) {
		प्रारूप_लिख("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, सदिशम्,
				lapic_test_delivery_val(tmr, सदिशम्) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, सदिशम्) ? 1 : 0,
				lapic_test_delivery_val(isr, सदिशम्) ? 1 : 0);
	} अन्यथा {
		प्रारूप_लिख("IRQ %d vec %d irr %d\n",
				irq, सदिशम्,
				lapic_test_delivery_val(irr, सदिशम्) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_सारणी + intr->pin * 2);
	प्रारूप_लिख("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->सदिशम्,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

शून्य dump_apic_irq_state(शून्य)
{
	पूर्णाङ्क irq;

	प्रारूप_लिख("--- IRQs state dump ---\n");
	प्रति (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_स्थितिः(irq);
	}
	प्रारूप_लिख("--- all ---\n");
}
