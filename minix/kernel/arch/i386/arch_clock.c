/* i386-specific clock functions. */

#ಅನ್ತರ್ಭಾವ <machine/ports.h>

#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ "kernel/interrupt.h"
#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ <sys/sched.h> /* for CP_*, CPUSTATES */
#ಆದರೆ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#ದೋಷ "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
#ಅನ್ತರ್ಭಾವ "apic.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
#ಅನ್ತರ್ಭಾವ "kernel/smp.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CLOCK_ACK_ಬಿಟ್   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#ವ್ಯಾಖ್ಯಾನಿಸು COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#ವ್ಯಾಖ್ಯಾನಿಸು LATCH_ಎಣಿಕೆ     0x00    /* cc00xxxx, c = channel, x = any */
#ವ್ಯಾಖ್ಯಾನಿಸು ವರ್ಗಫಲ_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_ಎಣಿಕೆ(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ಸ್ಥಿರ irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ probe_ticks;
ಸ್ಥಿರ u64_t tsc0, tsc1;
#ವ್ಯಾಖ್ಯಾನಿಸು PROBE_TICKS	(system_hz / 10)

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ tsc_ಪ್ರತಿ_ms[CONFIG_MAX_CPUS];
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ tsc_ಪ್ರತಿ_tick[CONFIG_MAX_CPUS];
ಸ್ಥಿರ uint64_t tsc_ಪ್ರತಿ_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ init_8253A_timer(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_ಬಹುಲಕ, ವರ್ಗಫಲ_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_ಎಣಿಕೆ(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_ಎಣಿಕೆ(freq) >> 8); /* timer high byte */

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
ಶೂನ್ಯ stop_8253A_timer(ಶೂನ್ಯ)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_ಬಹುಲಕ, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

ಶೂನ್ಯ arch_timer_int_handler(ಶೂನ್ಯ)
{
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ calib_cpu_handler(irq_hook_t * ಬಳಸದ(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	ಆದರೆ (probe_ticks == 1) {
		tsc0 = tsc;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ estimate_cpu_freq(ಶೂನ್ಯ)
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
	ಆಗ(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_ಬೀಗ();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_ಗಣ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

ಪೂರ್ಣಾಂಕ init_local_timer(ಚಿಹ್ನೆರಹಿತ freq)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	ಆದರೆ (lapic_addr) {
		ಚಿಹ್ನೆರಹಿತ cpu = cpuid;
		tsc_ಪ್ರತಿ_ms[cpu] = (ಚಿಹ್ನೆರಹಿತ)(cpu_get_freq(cpu) / 1000);
		tsc_ಪ್ರತಿ_tick[cpu] = (ಚಿಹ್ನೆರಹಿತ)(cpu_get_freq(cpu) / system_hz);
		lapic_ಗಣ_timer_one_shot(1000000 / system_hz);
	} ಇಲ್ಲದಿದ್ದರೆ {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#ಇಲ್ಲದಿದ್ದರೆ
	{
#ಆದರೆ_ಅಂತ್ಯ
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_ಪ್ರತಿ_ms[0] = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(cpu_get_freq(0) / 1000);
		tsc_ಪ್ರತಿ_tick[0] = (ಚಿಹ್ನೆರಹಿತ)(cpu_get_freq(0) / system_hz);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ stop_local_timer(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	ಆದರೆ (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
		stop_8253A_timer();
	}
}

ಶೂನ್ಯ restart_local_timer(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	ಆದರೆ (lapic_addr) {
		lapic_restart_timer();
	}
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ ರಿಜಿಸ್ಟರ್_local_timer_handler(ಸ್ಥಿರಾಂಕ irq_handler_t handler)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
	ಆದರೆ (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(ಬರೆ("Using LAPIC timer as tick source\n"));
	} ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ ಚಕ್ರಗಳು_accounting_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;
#ಆದರೆ_ಅಂತ್ಯ

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_ಕೊನೆಯ_tsc) = 0;
	get_cpu_var(cpu, cpu_ಕೊನೆಯ_idle) = 0;
}

ಶೂನ್ಯ context_stop(ರಚನೆ proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpu, tpt, counter;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಪೂರ್ಣಾಂಕ must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	ಆದರೆ (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_ಚಕ್ರಗಳು = p->p_ಚಕ್ರಗಳು + tmp;
		must_bkl_unlock = 1;
	} ಇಲ್ಲದಿದ್ದರೆ {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_ಬೀಗ.val;
		
		BKL_ಬೀಗ();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_ಚಕ್ರಗಳು = p->p_ಚಕ್ರಗಳು + tsc - *__tsc_ctr_switch;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
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
#ಆದರೆ_ಅಂತ್ಯ
	}
#ಇಲ್ಲದಿದ್ದರೆ
	read_tsc_64(&tsc);
	p->p_ಚಕ್ರಗಳು = p->p_ಚಕ್ರಗಳು + tsc - *__tsc_ctr_switch;
	cpu = 0;
#ಆದರೆ_ಅಂತ್ಯ

	tsc_delta = tsc - *__tsc_ctr_switch;

	ಆದರೆ (kbill_ipc) {
		kbill_ipc->p_kipc_ಚಕ್ರಗಳು += tsc_delta;
		kbill_ipc = ಖಾಲಿ;
	}

	ಆದರೆ (kbill_kcall) {
		kbill_kcall->p_kcall_ಚಕ್ರಗಳು += tsc_delta;
		kbill_kcall = ಖಾಲಿ;
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
	tpt = tsc_ಪ್ರತಿ_tick[cpu];

	p->p_tick_ಚಕ್ರಗಳು += tsc_delta;
	ಆಗ (tpt > 0 && p->p_tick_ಚಕ್ರಗಳು >= tpt) {
		p->p_tick_ಚಕ್ರಗಳು -= tpt;

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
	ಆದರೆ (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		ಆದರೆ (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		ಇಲ್ಲದಿದ್ದರೆ
			counter = CP_USER;

#ಆದರೆ DEBUG_RACE
		p->p_cpu_ಸಮಯ_ಎಡ = 0;
#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (tsc_delta < p->p_cpu_ಸಮಯ_ಎಡ) {
			p->p_cpu_ಸಮಯ_ಎಡ -= tsc_delta;
		} ಇಲ್ಲದಿದ್ದರೆ {
			p->p_cpu_ಸಮಯ_ಎಡ = 0;
		}
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		ಆದರೆ (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		ಇಲ್ಲದಿದ್ದರೆ
			counter = CP_INTR;
	}

	tsc_ಪ್ರತಿ_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ context_stop_idle(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ is_idle;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;
#ಆದರೆ_ಅಂತ್ಯ

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	ಆದರೆ (is_idle)
		restart_local_timer();
#ಆದರೆ SPROFILE
	ಆದರೆ (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#ಆದರೆ_ಅಂತ್ಯ
}

u64_t ms_2_cpu_ಸಮಯ(ಚಿಹ್ನೆರಹಿತ ms)
{
	ಹಿಂತಿರುಗಿ (u64_t)tsc_ಪ್ರತಿ_ms[cpuid] * ms;
}

ಚಿಹ್ನೆರಹಿತ cpu_ಸಮಯ_2_ms(u64_t cpu_ಸಮಯ)
{
	ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(cpu_ಸಮಯ / tsc_ಪ್ರತಿ_ms[cpuid]);
}

ಗಿಡ್ಡ cpu_ತುಂಬು(ಶೂನ್ಯ)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	ರಚನೆ proc *idle;
	ಗಿಡ್ಡ ತುಂಬು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;
#ಆದರೆ_ಅಂತ್ಯ

	u64_t *ಕೊನೆಯ_tsc, *ಕೊನೆಯ_idle;

	ಕೊನೆಯ_tsc = get_cpu_var_ptr(cpu, cpu_ಕೊನೆಯ_tsc);
	ಕೊನೆಯ_idle = get_cpu_var_ptr(cpu, cpu_ಕೊನೆಯ_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_ಚಕ್ರಗಳು; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	ಆದರೆ (*ಕೊನೆಯ_tsc) {
		tsc_delta = current_tsc - *ಕೊನೆಯ_tsc;
		idle_delta = *current_idle - *ಕೊನೆಯ_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		ತುಂಬು = ex64lo(busy / tsc_delta);

		ಆದರೆ (ತುಂಬು > 100)
			ತುಂಬು = 100;
	} ಇಲ್ಲದಿದ್ದರೆ
		ತುಂಬು = 0;

	*ಕೊನೆಯ_tsc = current_tsc;
	*ಕೊನೆಯ_idle = *current_idle;
	ಹಿಂತಿರುಗಿ ತುಂಬು;
}

ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಂಕ ms)
{
	u64_t ಚಕ್ರಗಳು = ms_2_cpu_ಸಮಯ(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + ಚಕ್ರಗಳು;
	ಮಾಡು { read_tsc_64(&tsc); } ಆಗ(tsc < tsc1);
	ಹಿಂತಿರುಗಿ;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
ಶೂನ್ಯ
get_cpu_ticks(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpu, uint64_t ticks[CPUSTATES])
{
	ಪೂರ್ಣಾಂಕ i;

	/* TODO: make this inter-CPU safe! */
	ಗಾಗಿ (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_ಪ್ರತಿ_state[cpu][i] / tsc_ಪ್ರತಿ_tick[cpu];
}
