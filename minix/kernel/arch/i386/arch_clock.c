/* i386-specific clock functions. */

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/ports.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/interrupt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sched.h> /* for CP_*, CPUSTATES */
#ਜੇ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#ਗਲਤੀ "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/spinlock.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/smp.h"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLOCK_ACK_ਦੋਆਧਾਰੀ_ਅੰਕ   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LATCH_ਗਿਣਤੀ     0x00    /* cc00xxxx, c = channel, x = any */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਰਗਫਲ_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_ਗਿਣਤੀ(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ਸਥਿਰ irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ probe_ticks;
ਸਥਿਰ u64_t tsc0, tsc1;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROBE_TICKS	(system_hz / 10)

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ tsc_ਪ੍ਰਤੀ_ms[CONFIG_MAX_CPUS];
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ tsc_ਪ੍ਰਤੀ_tick[CONFIG_MAX_CPUS];
ਸਥਿਰ uint64_t tsc_ਪ੍ਰਤੀ_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
ਪੂਰਨ init_8253A_timer(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_ਬਹੁਲਕ, ਵਰਗਫਲ_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_ਗਿਣਤੀ(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_ਗਿਣਤੀ(freq) >> 8); /* timer high byte */

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
ਖਾਲੀ stop_8253A_timer(ਖਾਲੀ)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_ਬਹੁਲਕ, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

ਖਾਲੀ arch_timer_int_handler(ਖਾਲੀ)
{
}

ਸਥਿਰ ਪੂਰਨ calib_cpu_handler(irq_hook_t * ਅਣਵਰਤਿਆ(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	ਜੇ (probe_ticks == 1) {
		tsc0 = tsc;
	}
	ਨਹੀਂਤਾਂ ਜੇ (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ estimate_cpu_freq(ਖਾਲੀ)
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
	ਜਦੋਂ(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_ਤਾਲਾ();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_ਸੈੱਟ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

ਪੂਰਨ init_local_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	ਜੇ (lapic_addr) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;
		tsc_ਪ੍ਰਤੀ_ms[cpu] = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(cpu_get_freq(cpu) / 1000);
		tsc_ਪ੍ਰਤੀ_tick[cpu] = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(cpu_get_freq(cpu) / system_hz);
		lapic_ਸੈੱਟ_timer_one_shot(1000000 / system_hz);
	} ਨਹੀਂਤਾਂ {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਅੰਤ
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_ਪ੍ਰਤੀ_ms[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(cpu_get_freq(0) / 1000);
		tsc_ਪ੍ਰਤੀ_tick[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(cpu_get_freq(0) / system_hz);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ stop_local_timer(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	ਜੇ (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		stop_8253A_timer();
	}
}

ਖਾਲੀ restart_local_timer(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	ਜੇ (lapic_addr) {
		lapic_restart_timer();
	}
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ ਰਜਿਸਟਰ_local_timer_handler(ਅਚਲ irq_handler_t handler)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
	ਜੇ (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(ਲਿਖੋ("Using LAPIC timer as tick source\n"));
	} ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ ਚੱਕਰਾਂ_accounting_init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;
#ਜੇ_ਅੰਤ

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_ਆਖਰੀ_tsc) = 0;
	get_cpu_var(cpu, cpu_ਆਖਰੀ_idle) = 0;
}

ਖਾਲੀ context_stop(ਬਣਤਰ proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpu, tpt, counter;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਪੂਰਨ must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	ਜੇ (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_ਚੱਕਰਾਂ = p->p_ਚੱਕਰਾਂ + tmp;
		must_bkl_unlock = 1;
	} ਨਹੀਂਤਾਂ {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_ਤਾਲਾ.val;
		
		BKL_ਤਾਲਾ();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_ਚੱਕਰਾਂ = p->p_ਚੱਕਰਾਂ + tsc - *__tsc_ctr_switch;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
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
#ਜੇ_ਅੰਤ
	}
#ਨਹੀਂਤਾਂ
	read_tsc_64(&tsc);
	p->p_ਚੱਕਰਾਂ = p->p_ਚੱਕਰਾਂ + tsc - *__tsc_ctr_switch;
	cpu = 0;
#ਜੇ_ਅੰਤ

	tsc_delta = tsc - *__tsc_ctr_switch;

	ਜੇ (kbill_ipc) {
		kbill_ipc->p_kipc_ਚੱਕਰਾਂ += tsc_delta;
		kbill_ipc = ਨਿਰਾ;
	}

	ਜੇ (kbill_kcall) {
		kbill_kcall->p_kcall_ਚੱਕਰਾਂ += tsc_delta;
		kbill_kcall = ਨਿਰਾ;
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
	tpt = tsc_ਪ੍ਰਤੀ_tick[cpu];

	p->p_tick_ਚੱਕਰਾਂ += tsc_delta;
	ਜਦੋਂ (tpt > 0 && p->p_tick_ਚੱਕਰਾਂ >= tpt) {
		p->p_tick_ਚੱਕਰਾਂ -= tpt;

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
	ਜੇ (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		ਜੇ (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		ਨਹੀਂਤਾਂ ਜੇ (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		ਨਹੀਂਤਾਂ
			counter = CP_USER;

#ਜੇ DEBUG_RACE
		p->p_cpu_ਸਮਾਂ_ਖੱਬਾ = 0;
#ਨਹੀਂਤਾਂ
		ਜੇ (tsc_delta < p->p_cpu_ਸਮਾਂ_ਖੱਬਾ) {
			p->p_cpu_ਸਮਾਂ_ਖੱਬਾ -= tsc_delta;
		} ਨਹੀਂਤਾਂ {
			p->p_cpu_ਸਮਾਂ_ਖੱਬਾ = 0;
		}
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		ਜੇ (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		ਨਹੀਂਤਾਂ
			counter = CP_INTR;
	}

	tsc_ਪ੍ਰਤੀ_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ context_stop_idle(ਖਾਲੀ)
{
	ਪੂਰਨ is_idle;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;
#ਜੇ_ਅੰਤ

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	ਜੇ (is_idle)
		restart_local_timer();
#ਜੇ SPROFILE
	ਜੇ (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#ਜੇ_ਅੰਤ
}

u64_t ms_2_cpu_ਸਮਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ms)
{
	ਵਾਪਸ (u64_t)tsc_ਪ੍ਰਤੀ_ms[cpuid] * ms;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ cpu_ਸਮਾਂ_2_ms(u64_t cpu_ਸਮਾਂ)
{
	ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(cpu_ਸਮਾਂ / tsc_ਪ੍ਰਤੀ_ms[cpuid]);
}

ਛੋਟਾ cpu_ਲੱਦੋ(ਖਾਲੀ)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	ਬਣਤਰ proc *idle;
	ਛੋਟਾ ਲੱਦੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;
#ਜੇ_ਅੰਤ

	u64_t *ਆਖਰੀ_tsc, *ਆਖਰੀ_idle;

	ਆਖਰੀ_tsc = get_cpu_var_ptr(cpu, cpu_ਆਖਰੀ_tsc);
	ਆਖਰੀ_idle = get_cpu_var_ptr(cpu, cpu_ਆਖਰੀ_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_ਚੱਕਰਾਂ; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	ਜੇ (*ਆਖਰੀ_tsc) {
		tsc_delta = current_tsc - *ਆਖਰੀ_tsc;
		idle_delta = *current_idle - *ਆਖਰੀ_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		ਲੱਦੋ = ex64lo(busy / tsc_delta);

		ਜੇ (ਲੱਦੋ > 100)
			ਲੱਦੋ = 100;
	} ਨਹੀਂਤਾਂ
		ਲੱਦੋ = 0;

	*ਆਖਰੀ_tsc = current_tsc;
	*ਆਖਰੀ_idle = *current_idle;
	ਵਾਪਸ ਲੱਦੋ;
}

ਖਾਲੀ busy_delay_ms(ਪੂਰਨ ms)
{
	u64_t ਚੱਕਰਾਂ = ms_2_cpu_ਸਮਾਂ(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + ਚੱਕਰਾਂ;
	ਕਰੋ { read_tsc_64(&tsc); } ਜਦੋਂ(tsc < tsc1);
	ਵਾਪਸ;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
ਖਾਲੀ
get_cpu_ticks(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpu, uint64_t ticks[CPUSTATES])
{
	ਪੂਰਨ i;

	/* TODO: make this inter-CPU safe! */
	ਲਈ (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_ਪ੍ਰਤੀ_state[cpu][i] / tsc_ਪ੍ਰਤੀ_tick[cpu];
}
