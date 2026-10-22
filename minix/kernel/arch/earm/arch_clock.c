/* ARM-specific clock functions. */

#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ "kernel/clock.h"
#అన్తర్భావ "kernel/interrupt.h"
#అన్తర్భావ <minix/u64.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ "kernel/glo.h"
#అన్తర్భావ "kernel/profile.h"

#అన్తర్భావ <sys/sched.h> /* for CP_*, CPUSTATES */
#ఒకవేళ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#దోషం "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#అయితే_ముగింపు

#అన్తర్భావ "kernel/spinlock.h"

#నిర్వచితమైతే CONFIG_SMP
#అన్తర్భావ "kernel/smp.h"
#దోషం CONFIG_SMP is unsupported on ARM
#అయితే_ముగింపు

#అన్తర్భావ "bsp_timer.h"
#అన్తర్భావ "bsp_intr.h"

స్థిర గుర్తులేని tsc_చొప్పున_ms[CONFIG_MAX_CPUS];
స్థిర గుర్తులేని tsc_చొప్పున_tick[CONFIG_MAX_CPUS];
స్థిర uint64_t tsc_చొప్పున_state[CONFIG_MAX_CPUS][CPUSTATES];

పూర్ణాంకం init_local_timer(గుర్తులేని freq)
{
	bsp_timer_init(freq);

	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		tsc_చొప్పున_ms[0] = 16250;
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		tsc_చొప్పున_ms[0] = 15000;
	} లేకపోతే {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_చొప్పున_tick[0] = tsc_చొప్పున_ms[0] * 1000 / system_hz;

	తిరిగి 0;
}

శూన్య stop_local_timer(శూన్య)
{
	bsp_timer_stop();
}

శూన్య arch_timer_int_handler(శూన్య)
{
	bsp_timer_int_handler();
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
#దోషం CONFIG_SMP is unsupported on ARM
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

శూన్య restart_local_timer(శూన్య)
{
}

పూర్ణాంకం రిజిస్టరు_local_timer_handler(స్థిరరాశి irq_handler_t handler)
{
	తిరిగి bsp_రిజిస్టరు_timer_handler(handler);
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
