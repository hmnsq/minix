/* i386-specific clock functions. */

#ଅନ୍ତର୍ଭାଵ <machine/ports.h>

#ଅନ୍ତର୍ଭାଵ "kernel/clock.h"
#ଅନ୍ତର୍ଭାଵ "kernel/interrupt.h"
#ଅନ୍ତର୍ଭାଵ <minix/u64.h>

#ଅନ୍ତର୍ଭାଵ <sys/sched.h> /* for CP_*, CPUSTATES */
#ଯଦି CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#error "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#endif

#ifdef USE_APIC
#ଅନ୍ତର୍ଭାଵ "apic.h"
#endif

#ଅନ୍ତର୍ଭାଵ "kernel/spinlock.h"

#ifdef CONFIG_SMP
#ଅନ୍ତର୍ଭାଵ "kernel/smp.h"
#endif

#define CLOCK_ACK_BIT   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#define COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#define LATCH_ଗଣନା     0x00    /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#define TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#define TIMER_ଗଣନା(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ସ୍ଥିର irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ସ୍ଥିର unsigned probe_ticks;
ସ୍ଥିର u64_t tsc0, tsc1;
#define PROBE_TICKS	(system_hz / 10)

ସ୍ଥିର unsigned tsc_per_ms[CONFIG_MAX_CPUS];
ସ୍ଥିର unsigned tsc_per_tick[CONFIG_MAX_CPUS];
ସ୍ଥିର uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_8253A_timer(ଅଚଳ unsigned freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_MODE, SQUARE_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_ଗଣନା(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_ଗଣନା(freq) >> 8); /* timer high byte */

	ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
ଶୂନ୍ୟ stop_8253A_timer(ଶୂନ୍ୟ)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_MODE, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

ଶୂନ୍ୟ arch_timer_int_handler(ଶୂନ୍ୟ)
{
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା calib_cpu_handler(irq_hook_t * UNUSED(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	ଯଦି (probe_ticks == 1) {
		tsc0 = tsc;
	}
	ଅନ୍ୟଥା ଯଦି (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ estimate_cpu_freq(ଶୂନ୍ୟ)
{
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_cpu;

	/* set the probe, we use the legacy timer, IRQ 0 */
	put_irq_handler(&calib_cpu, CLOCK_IRQ, calib_cpu_handler);

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	/* set the PIC timer to get some time */
	intr_enable();

	/* loop for some time to get a sample */
	ଯେତେବେଳେ(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_LOCK();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_set_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (unsigned long)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_local_timer(unsigned freq)
{
#ifdef USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	ଯଦି (lapic_addr) {
		unsigned cpu = cpuid;
		tsc_per_ms[cpu] = (unsigned)(cpu_get_freq(cpu) / 1000);
		tsc_per_tick[cpu] = (unsigned)(cpu_get_freq(cpu) / system_hz);
		lapic_set_timer_one_shot(1000000 / system_hz);
	} ଅନ୍ୟଥା {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#ଅନ୍ୟଥା
	{
#endif
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_per_ms[0] = (unsigned long)(cpu_get_freq(0) / 1000);
		tsc_per_tick[0] = (unsigned)(cpu_get_freq(0) / system_hz);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ stop_local_timer(ଶୂନ୍ୟ)
{
#ifdef USE_APIC
	ଯଦି (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} ଅନ୍ୟଥା
#endif
	{
		stop_8253A_timer();
	}
}

ଶୂନ୍ୟ restart_local_timer(ଶୂନ୍ୟ)
{
#ifdef USE_APIC
	ଯଦି (lapic_addr) {
		lapic_restart_timer();
	}
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା register_local_timer_handler(ଅଚଳ irq_handler_t handler)
{
#ifdef USE_APIC
	ଯଦି (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(printf("Using LAPIC timer as tick source\n"));
	} ଅନ୍ୟଥା
#endif
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ cycles_accounting_init(ଶୂନ୍ୟ)
{
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_last_tsc) = 0;
	get_cpu_var(cpu, cpu_last_idle) = 0;
}

ଶୂନ୍ୟ context_stop(ସଂରଚନା proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu, tpt, counter;
#ifdef CONFIG_SMP
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	ଯଦି (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_cycles = p->p_cycles + tmp;
		must_bkl_unlock = 1;
	} ଅନ୍ୟଥା {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_lock.val;
		
		BKL_LOCK();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;

#ifdef CONFIG_SMP
		/*
		 * Since at the time we got a scheduling IPI we might have been
		 * waiting for BKL already, we may miss it due to a similar IPI to
		 * the cpu which is already waiting for us to handle its. This
		 * results in a live-lock of these two cpus.
		 *
		 * Therefore we always check if there is one pending and if so,
		 * we handle it straight away so the other cpu can continue and
		 * we do not deadlock.
		 */
		smp_sched_handler();
#endif
	}
#ଅନ୍ୟଥା
	read_tsc_64(&tsc);
	p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;
	cpu = 0;
#endif

	tsc_delta = tsc - *__tsc_ctr_switch;

	ଯଦି (kbill_ipc) {
		kbill_ipc->p_kipc_cycles += tsc_delta;
		kbill_ipc = ଖାଲି;
	}

	ଯଦି (kbill_kcall) {
		kbill_kcall->p_kcall_cycles += tsc_delta;
		kbill_kcall = ଖାଲି;
	}

	/*
	 * Perform CPU average accounting here, rather than in the generic
	 * clock handler.  Doing it here offers two advantages: 1) we can
	 * account for time spent in the kernel, and 2) we properly account for
	 * CPU time spent by a process that has a lot of short-lasting activity
	 * such that it spends serious CPU time but never actually runs when a
	 * clock tick triggers.  Note that clock speed inaccuracy requires that
	 * the code below is a loop, but the loop will in by far most cases not
	 * be executed more than once, and often be skipped at all.
	 */
	tpt = tsc_per_tick[cpu];

	p->p_tick_cycles += tsc_delta;
	ଯେତେବେଳେ (tpt > 0 && p->p_tick_cycles >= tpt) {
		p->p_tick_cycles -= tpt;

		/*
		 * The process has spent roughly a whole clock tick worth of
		 * CPU cycles.  Update its per-process CPU utilization counter.
		 * Some of the cycles may actually have been spent in a
		 * previous second, but that is not a problem.
		 */
		cpuavg_increment(&p->p_cpuavg, kclockinfo.uptime, system_hz);
	}

	/*
	 * deduct the just consumed cpu cycles from the cpu time left for this
	 * process during its current quantum. Skip IDLE and other pseudo kernel
	 * tasks, except for global accounting purposes.
	 */
	ଯଦି (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		ଯଦି (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		ଅନ୍ୟଥା ଯଦି (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		ଅନ୍ୟଥା
			counter = CP_USER;

#ଯଦି DEBUG_RACE
		p->p_cpu_time_ବାମ = 0;
#ଅନ୍ୟଥା
		ଯଦି (tsc_delta < p->p_cpu_time_ବାମ) {
			p->p_cpu_time_ବାମ -= tsc_delta;
		} ଅନ୍ୟଥା {
			p->p_cpu_time_ବାମ = 0;
		}
#endif
	} ଅନ୍ୟଥା {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		ଯଦି (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		ଅନ୍ୟଥା
			counter = CP_INTR;
	}

	tsc_per_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;

#ifdef CONFIG_SMP
	ଯଦି(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#endif
}

ଶୂନ୍ୟ context_stop_idle(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_idle;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	ଯଦି (is_idle)
		restart_local_timer();
#ଯଦି SPROFILE
	ଯଦି (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#endif
}

u64_t ms_2_cpu_time(unsigned ms)
{
	ଫେରନ୍ତୁ (u64_t)tsc_per_ms[cpuid] * ms;
}

unsigned cpu_time_2_ms(u64_t cpu_time)
{
	ଫେରନ୍ତୁ (unsigned long)(cpu_time / tsc_per_ms[cpuid]);
}

short cpu_load(ଶୂନ୍ୟ)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	ସଂରଚନା proc *idle;
	short load;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	u64_t *last_tsc, *last_idle;

	last_tsc = get_cpu_var_ptr(cpu, cpu_last_tsc);
	last_idle = get_cpu_var_ptr(cpu, cpu_last_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_cycles; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	ଯଦି (*last_tsc) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = *current_idle - *last_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		load = ex64lo(busy / tsc_delta);

		ଯଦି (load > 100)
			load = 100;
	} ଅନ୍ୟଥା
		load = 0;

	*last_tsc = current_tsc;
	*last_idle = *current_idle;
	ଫେରନ୍ତୁ load;
}

ଶୂନ୍ୟ busy_delay_ms(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ms)
{
	u64_t cycles = ms_2_cpu_time(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + cycles;
	କର { read_tsc_64(&tsc); } ଯେତେବେଳେ(tsc < tsc1);
	ଫେରନ୍ତୁ;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
ଶୂନ୍ୟ
get_cpu_ticks(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu, uint64_t ticks[CPUSTATES])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	/* TODO: make this inter-CPU safe! */
	ପାଇଁ (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_per_state[cpu][i] / tsc_per_tick[cpu];
}
