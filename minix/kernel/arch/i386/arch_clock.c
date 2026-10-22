/* i386-specific clock functions. */

#అన్తర్భావ <machine/ports.h>

#అన్తర్భావ "kernel/clock.h"
#అన్తర్భావ "kernel/interrupt.h"
#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ <sys/sched.h> /* for CP_*, CPUSTATES */
#ఒకవేళ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#దోషం "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#అయితే_ముగింపు

#నిర్వచితమైతే USE_APIC
#అన్తర్భావ "apic.h"
#అయితే_ముగింపు

#అన్తర్భావ "kernel/spinlock.h"

#నిర్వచితమైతే CONFIG_SMP
#అన్తర్భావ "kernel/smp.h"
#అయితే_ముగింపు

#నిర్వచించు CLOCK_ACK_బిట్టు   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#నిర్వచించు COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#నిర్వచించు LATCH_లెక్క     0x00    /* cc00xxxx, c = channel, x = any */
#నిర్వచించు వర్గం_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#నిర్వచించు TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#నిర్వచించు TIMER_లెక్క(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

స్థిర irq_hook_t pic_timer_hook;		/* interrupt handler hook */

స్థిర గుర్తులేని probe_ticks;
స్థిర u64_t tsc0, tsc1;
#నిర్వచించు PROBE_TICKS	(system_hz / 10)

స్థిర గుర్తులేని tsc_చొప్పున_ms[CONFIG_MAX_CPUS];
స్థిర గుర్తులేని tsc_చొప్పున_tick[CONFIG_MAX_CPUS];
స్థిర uint64_t tsc_చొప్పున_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
పూర్ణాంకం init_8253A_timer(స్థిరరాశి గుర్తులేని freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_బహుళకం, వర్గం_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_లెక్క(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_లెక్క(freq) >> 8); /* timer high byte */

	తిరిగి OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
శూన్య stop_8253A_timer(శూన్య)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_బహుళకం, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

శూన్య arch_timer_int_handler(శూన్య)
{
}

స్థిర పూర్ణాంకం calib_cpu_handler(irq_hook_t * వాడని(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	ఒకవేళ (probe_ticks == 1) {
		tsc0 = tsc;
	}
	లేకపోతే ఒకవేళ (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	తిరిగి 1;
}

స్థిర శూన్య estimate_cpu_freq(శూన్య)
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
	అయితే(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_తాళం();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_సమితి_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (గుర్తులేని దీర్ఘ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

పూర్ణాంకం init_local_timer(గుర్తులేని freq)
{
#నిర్వచితమైతే USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	ఒకవేళ (lapic_addr) {
		గుర్తులేని cpu = cpuid;
		tsc_చొప్పున_ms[cpu] = (గుర్తులేని)(cpu_get_freq(cpu) / 1000);
		tsc_చొప్పున_tick[cpu] = (గుర్తులేని)(cpu_get_freq(cpu) / system_hz);
		lapic_సమితి_timer_one_shot(1000000 / system_hz);
	} లేకపోతే {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#లేకపోతే
	{
#అయితే_ముగింపు
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_చొప్పున_ms[0] = (గుర్తులేని దీర్ఘ)(cpu_get_freq(0) / 1000);
		tsc_చొప్పున_tick[0] = (గుర్తులేని)(cpu_get_freq(0) / system_hz);
	}

	తిరిగి 0;
}

శూన్య stop_local_timer(శూన్య)
{
#నిర్వచితమైతే USE_APIC
	ఒకవేళ (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} లేకపోతే
#అయితే_ముగింపు
	{
		stop_8253A_timer();
	}
}

శూన్య restart_local_timer(శూన్య)
{
#నిర్వచితమైతే USE_APIC
	ఒకవేళ (lapic_addr) {
		lapic_restart_timer();
	}
#అయితే_ముగింపు
}

పూర్ణాంకం రిజిస్టరు_local_timer_handler(స్థిరరాశి irq_handler_t handler)
{
#నిర్వచితమైతే USE_APIC
	ఒకవేళ (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(రాయి("Using LAPIC timer as tick source\n"));
	} లేకపోతే
#అయితే_ముగింపు
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	తిరిగి 0;
}

శూన్య చక్రాలు_accounting_init(శూన్య)
{
#నిర్వచితమైతే CONFIG_SMP
	గుర్తులేని cpu = cpuid;
#అయితే_ముగింపు

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_చివరి_tsc) = 0;
	get_cpu_var(cpu, cpu_చివరి_idle) = 0;
}

శూన్య context_stop(నిర్మాణం proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	గుర్తులేని పూర్ణాంకం cpu, tpt, counter;
#నిర్వచితమైతే CONFIG_SMP
	పూర్ణాంకం must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	ఒకవేళ (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_చక్రాలు = p->p_చక్రాలు + tmp;
		must_bkl_unlock = 1;
	} లేకపోతే {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_తాళం.val;
		
		BKL_తాళం();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_చక్రాలు = p->p_చక్రాలు + tsc - *__tsc_ctr_switch;

#నిర్వచితమైతే CONFIG_SMP
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
#అయితే_ముగింపు
	}
#లేకపోతే
	read_tsc_64(&tsc);
	p->p_చక్రాలు = p->p_చక్రాలు + tsc - *__tsc_ctr_switch;
	cpu = 0;
#అయితే_ముగింపు

	tsc_delta = tsc - *__tsc_ctr_switch;

	ఒకవేళ (kbill_ipc) {
		kbill_ipc->p_kipc_చక్రాలు += tsc_delta;
		kbill_ipc = ఖాళీ;
	}

	ఒకవేళ (kbill_kcall) {
		kbill_kcall->p_kcall_చక్రాలు += tsc_delta;
		kbill_kcall = ఖాళీ;
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
	tpt = tsc_చొప్పున_tick[cpu];

	p->p_tick_చక్రాలు += tsc_delta;
	అయితే (tpt > 0 && p->p_tick_చక్రాలు >= tpt) {
		p->p_tick_చక్రాలు -= tpt;

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
	ఒకవేళ (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		ఒకవేళ (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		లేకపోతే ఒకవేళ (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		లేకపోతే
			counter = CP_USER;

#ఒకవేళ DEBUG_RACE
		p->p_cpu_సమయం_ఎడమ = 0;
#లేకపోతే
		ఒకవేళ (tsc_delta < p->p_cpu_సమయం_ఎడమ) {
			p->p_cpu_సమయం_ఎడమ -= tsc_delta;
		} లేకపోతే {
			p->p_cpu_సమయం_ఎడమ = 0;
		}
#అయితే_ముగింపు
	} లేకపోతే {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		ఒకవేళ (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		లేకపోతే
			counter = CP_INTR;
	}

	tsc_చొప్పున_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;

#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#అయితే_ముగింపు
}

శూన్య context_stop_idle(శూన్య)
{
	పూర్ణాంకం is_idle;
#నిర్వచితమైతే CONFIG_SMP
	గుర్తులేని cpu = cpuid;
#అయితే_ముగింపు

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	ఒకవేళ (is_idle)
		restart_local_timer();
#ఒకవేళ SPROFILE
	ఒకవేళ (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#అయితే_ముగింపు
}

u64_t ms_2_cpu_సమయం(గుర్తులేని ms)
{
	తిరిగి (u64_t)tsc_చొప్పున_ms[cpuid] * ms;
}

గుర్తులేని cpu_సమయం_2_ms(u64_t cpu_సమయం)
{
	తిరిగి (గుర్తులేని దీర్ఘ)(cpu_సమయం / tsc_చొప్పున_ms[cpuid]);
}

పొట్టి cpu_ఎక్కించు(శూన్య)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	నిర్మాణం proc *idle;
	పొట్టి ఎక్కించు;
#నిర్వచితమైతే CONFIG_SMP
	గుర్తులేని cpu = cpuid;
#అయితే_ముగింపు

	u64_t *చివరి_tsc, *చివరి_idle;

	చివరి_tsc = get_cpu_var_ptr(cpu, cpu_చివరి_tsc);
	చివరి_idle = get_cpu_var_ptr(cpu, cpu_చివరి_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_చక్రాలు; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	ఒకవేళ (*చివరి_tsc) {
		tsc_delta = current_tsc - *చివరి_tsc;
		idle_delta = *current_idle - *చివరి_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		ఎక్కించు = ex64lo(busy / tsc_delta);

		ఒకవేళ (ఎక్కించు > 100)
			ఎక్కించు = 100;
	} లేకపోతే
		ఎక్కించు = 0;

	*చివరి_tsc = current_tsc;
	*చివరి_idle = *current_idle;
	తిరిగి ఎక్కించు;
}

శూన్య busy_delay_ms(పూర్ణాంకం ms)
{
	u64_t చక్రాలు = ms_2_cpu_సమయం(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + చక్రాలు;
	చేయి { read_tsc_64(&tsc); } అయితే(tsc < tsc1);
	తిరిగి;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
శూన్య
get_cpu_ticks(గుర్తులేని పూర్ణాంకం cpu, uint64_t ticks[CPUSTATES])
{
	పూర్ణాంకం i;

	/* TODO: make this inter-CPU safe! */
	కొరకు (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_చొప్పున_state[cpu][i] / tsc_చొప్పున_tick[cpu];
}
