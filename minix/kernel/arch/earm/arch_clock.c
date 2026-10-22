/* ARM-specific clock functions. */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ "kernel/clock.h"
#ଅନ୍ତର୍ଭାଵ "kernel/interrupt.h"
#ଅନ୍ତର୍ଭାଵ <minix/u64.h>
#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ "kernel/glo.h"
#ଅନ୍ତର୍ଭାଵ "kernel/profile.h"

#ଅନ୍ତର୍ଭାଵ <sys/sched.h> /* for CP_*, CPUSTATES */
#ଯଦି CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#error "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#endif

#ଅନ୍ତର୍ଭାଵ "kernel/spinlock.h"

#ifdef CONFIG_SMP
#ଅନ୍ତର୍ଭାଵ "kernel/smp.h"
#error CONFIG_SMP is unsupported on ARM
#endif

#ଅନ୍ତର୍ଭାଵ "bsp_timer.h"
#ଅନ୍ତର୍ଭାଵ "bsp_intr.h"

ସ୍ଥିର unsigned tsc_per_ms[CONFIG_MAX_CPUS];
ସ୍ଥିର unsigned tsc_per_tick[CONFIG_MAX_CPUS];
ସ୍ଥିର uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_local_timer(unsigned freq)
{
	bsp_timer_init(freq);

	ଯଦି (BOARD_IS_BBXM(machine.board_id)) {
		tsc_per_ms[0] = 16250;
	} ଅନ୍ୟଥା ଯଦି (BOARD_IS_BB(machine.board_id)) {
		tsc_per_ms[0] = 15000;
	} ଅନ୍ୟଥା {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_per_tick[0] = tsc_per_ms[0] * 1000 / system_hz;

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ stop_local_timer(ଶୂନ୍ୟ)
{
	bsp_timer_stop();
}

ଶୂନ୍ୟ arch_timer_int_handler(ଶୂନ୍ୟ)
{
	bsp_timer_int_handler();
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
#error CONFIG_SMP is unsupported on ARM
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

ଶୂନ୍ୟ restart_local_timer(ଶୂନ୍ୟ)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା register_local_timer_handler(ଅଚଳ irq_handler_t handler)
{
	ଫେରନ୍ତୁ bsp_register_timer_handler(handler);
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
