/* ARM-specific clock functions. */

#അന്തര്ഭാവ "kernel/kernel.h"

#അന്തര്ഭാവ "kernel/clock.h"
#അന്തര്ഭാവ "kernel/interrupt.h"
#അന്തര്ഭാവ <minix/u64.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ "kernel/glo.h"
#അന്തര്ഭാവ "kernel/profile.h"

#അന്തര്ഭാവ <sys/sched.h> /* for CP_*, CPUSTATES */
#എങ്കിൽ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#error "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#endif

#അന്തര്ഭാവ "kernel/spinlock.h"

#ifdef CONFIG_SMP
#അന്തര്ഭാവ "kernel/smp.h"
#error CONFIG_SMP is unsupported on ARM
#endif

#അന്തര്ഭാവ "bsp_timer.h"
#അന്തര്ഭാവ "bsp_intr.h"

സ്ഥിതം unsigned tsc_per_ms[CONFIG_MAX_CPUS];
സ്ഥിതം unsigned tsc_per_tick[CONFIG_MAX_CPUS];
സ്ഥിതം uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

പൂർണ്ണസംഖ്യ init_local_timer(unsigned freq)
{
	bsp_timer_init(freq);

	എങ്കിൽ (BOARD_IS_BBXM(machine.board_id)) {
		tsc_per_ms[0] = 16250;
	} അല്ലെങ്കിൽ എങ്കിൽ (BOARD_IS_BB(machine.board_id)) {
		tsc_per_ms[0] = 15000;
	} അല്ലെങ്കിൽ {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_per_tick[0] = tsc_per_ms[0] * 1000 / system_hz;

	തിരികെ 0;
}

ശൂന്യം stop_local_timer(ശൂന്യം)
{
	bsp_timer_stop();
}

ശൂന്യം arch_timer_int_handler(ശൂന്യം)
{
	bsp_timer_int_handler();
}

ശൂന്യം cycles_accounting_init(ശൂന്യം)
{
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_last_tsc) = 0;
	get_cpu_var(cpu, cpu_last_idle) = 0;
}

ശൂന്യം context_stop(ഘടന proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	unsigned പൂർണ്ണസംഖ്യ cpu, tpt, counter;

#ifdef CONFIG_SMP
#error CONFIG_SMP is unsupported on ARM
#അല്ലെങ്കിൽ
	read_tsc_64(&tsc);
	p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;
	cpu = 0;
#endif

	tsc_delta = tsc - *__tsc_ctr_switch;

	എങ്കിൽ (kbill_ipc) {
		kbill_ipc->p_kipc_cycles += tsc_delta;
		kbill_ipc = ശൂന്യ;
	}

	എങ്കിൽ (kbill_kcall) {
		kbill_kcall->p_kcall_cycles += tsc_delta;
		kbill_kcall = ശൂന്യ;
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
	ആകുമ്പോൾ (tpt > 0 && p->p_tick_cycles >= tpt) {
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
	എങ്കിൽ (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		എങ്കിൽ (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		അല്ലെങ്കിൽ എങ്കിൽ (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		അല്ലെങ്കിൽ
			counter = CP_USER;

#എങ്കിൽ DEBUG_RACE
		p->p_cpu_time_ഇടത് = 0;
#അല്ലെങ്കിൽ
		എങ്കിൽ (tsc_delta < p->p_cpu_time_ഇടത്) {
			p->p_cpu_time_ഇടത് -= tsc_delta;
		} അല്ലെങ്കിൽ {
			p->p_cpu_time_ഇടത് = 0;
		}
#endif
	} അല്ലെങ്കിൽ {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		എങ്കിൽ (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		അല്ലെങ്കിൽ
			counter = CP_INTR;
	}

	tsc_per_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

ശൂന്യം context_stop_idle(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ is_idle;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	എങ്കിൽ (is_idle)
		restart_local_timer();
#എങ്കിൽ SPROFILE
	എങ്കിൽ (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#endif
}

ശൂന്യം restart_local_timer(ശൂന്യം)
{
}

പൂർണ്ണസംഖ്യ register_local_timer_handler(സ്ഥിരം irq_handler_t handler)
{
	തിരികെ bsp_register_timer_handler(handler);
}

u64_t ms_2_cpu_time(unsigned ms)
{
	തിരികെ (u64_t)tsc_per_ms[cpuid] * ms;
}

unsigned cpu_time_2_ms(u64_t cpu_time)
{
	തിരികെ (unsigned long)(cpu_time / tsc_per_ms[cpuid]);
}

short cpu_load(ശൂന്യം)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	ഘടന proc *idle;
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
	എങ്കിൽ (*last_tsc) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = *current_idle - *last_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		load = ex64lo(busy / tsc_delta);

		എങ്കിൽ (load > 100)
			load = 100;
	} അല്ലെങ്കിൽ
		load = 0;

	*last_tsc = current_tsc;
	*last_idle = *current_idle;
	തിരികെ load;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
ശൂന്യം
get_cpu_ticks(unsigned പൂർണ്ണസംഖ്യ cpu, uint64_t ticks[CPUSTATES])
{
	പൂർണ്ണസംഖ്യ i;

	/* TODO: make this inter-CPU safe! */
	വേണ്ടി (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_per_state[cpu][i] / tsc_per_tick[cpu];
}
