/* ARM-specific clock functions. */

#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव "kernel/clock.h"
#अन्तर्भाव "kernel/interrupt.h"
#अन्तर्भाव <minix/u64.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव "kernel/glo.h"
#अन्तर्भाव "kernel/profile.h"

#अन्तर्भाव <sys/sched.h> /* for CP_*, CPUSTATES */
#यदि CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#error "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#endif

#अन्तर्भाव "kernel/spinlock.h"

#ifdef CONFIG_SMP
#अन्तर्भाव "kernel/smp.h"
#error CONFIG_SMP is unsupported on ARM
#endif

#अन्तर्भाव "bsp_timer.h"
#अन्तर्भाव "bsp_intr.h"

स्थिर unsigned tsc_per_ms[CONFIG_MAX_CPUS];
स्थिर unsigned tsc_per_tick[CONFIG_MAX_CPUS];
स्थिर uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

पूर्णाङ्क init_local_timer(unsigned freq)
{
	bsp_timer_init(freq);

	यदि (BOARD_IS_BBXM(machine.board_id)) {
		tsc_per_ms[0] = 16250;
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		tsc_per_ms[0] = 15000;
	} अन्यथा {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_per_tick[0] = tsc_per_ms[0] * 1000 / system_hz;

	प्रत्यागम 0;
}

शून्य stop_local_timer(शून्य)
{
	bsp_timer_stop();
}

शून्य arch_timer_int_handler(शून्य)
{
	bsp_timer_int_handler();
}

शून्य cycles_accounting_init(शून्य)
{
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_last_tsc) = 0;
	get_cpu_var(cpu, cpu_last_idle) = 0;
}

शून्य context_stop(संरचना proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	unsigned पूर्णाङ्क cpu, tpt, counter;

#ifdef CONFIG_SMP
#error CONFIG_SMP is unsupported on ARM
#अन्यथा
	read_tsc_64(&tsc);
	p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;
	cpu = 0;
#endif

	tsc_delta = tsc - *__tsc_ctr_switch;

	यदि (kbill_ipc) {
		kbill_ipc->p_kipc_cycles += tsc_delta;
		kbill_ipc = रिक्त;
	}

	यदि (kbill_kcall) {
		kbill_kcall->p_kcall_cycles += tsc_delta;
		kbill_kcall = रिक्त;
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
	यावत् (tpt > 0 && p->p_tick_cycles >= tpt) {
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
	यदि (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		यदि (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		अन्यथा यदि (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		अन्यथा
			counter = CP_USER;

#यदि DEBUG_RACE
		p->p_cpu_time_डावा = 0;
#अन्यथा
		यदि (tsc_delta < p->p_cpu_time_डावा) {
			p->p_cpu_time_डावा -= tsc_delta;
		} अन्यथा {
			p->p_cpu_time_डावा = 0;
		}
#endif
	} अन्यथा {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		यदि (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		अन्यथा
			counter = CP_INTR;
	}

	tsc_per_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

शून्य context_stop_idle(शून्य)
{
	पूर्णाङ्क is_idle;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	यदि (is_idle)
		restart_local_timer();
#यदि SPROFILE
	यदि (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#endif
}

शून्य restart_local_timer(शून्य)
{
}

पूर्णाङ्क register_local_timer_handler(ध्रुव irq_handler_t handler)
{
	प्रत्यागम bsp_register_timer_handler(handler);
}

u64_t ms_2_cpu_time(unsigned ms)
{
	प्रत्यागम (u64_t)tsc_per_ms[cpuid] * ms;
}

unsigned cpu_time_2_ms(u64_t cpu_time)
{
	प्रत्यागम (unsigned long)(cpu_time / tsc_per_ms[cpuid]);
}

short cpu_load(शून्य)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	संरचना proc *idle;
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
	यदि (*last_tsc) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = *current_idle - *last_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		load = ex64lo(busy / tsc_delta);

		यदि (load > 100)
			load = 100;
	} अन्यथा
		load = 0;

	*last_tsc = current_tsc;
	*last_idle = *current_idle;
	प्रत्यागम load;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
शून्य
get_cpu_ticks(unsigned पूर्णाङ्क cpu, uint64_t ticks[CPUSTATES])
{
	पूर्णाङ्क i;

	/* TODO: make this inter-CPU safe! */
	प्रति (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_per_state[cpu][i] / tsc_per_tick[cpu];
}
