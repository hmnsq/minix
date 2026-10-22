/* ARM-specific clock functions. */

#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব "kernel/clock.h"
#অন্তর্ভাব "kernel/interrupt.h"
#অন্তর্ভাব <minix/u64.h>
#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব "kernel/glo.h"
#অন্তর্ভাব "kernel/profile.h"

#অন্তর্ভাব <sys/sched.h> /* for CP_*, CPUSTATES */
#যদি CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#error "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#endif

#অন্তর্ভাব "kernel/spinlock.h"

#ifdef CONFIG_SMP
#অন্তর্ভাব "kernel/smp.h"
#error CONFIG_SMP is unsupported on ARM
#endif

#অন্তর্ভাব "bsp_timer.h"
#অন্তর্ভাব "bsp_intr.h"

স্থির unsigned tsc_per_ms[CONFIG_MAX_CPUS];
স্থির unsigned tsc_per_tick[CONFIG_MAX_CPUS];
স্থির uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

পূর্ণসংখ্যা init_local_timer(unsigned freq)
{
	bsp_timer_init(freq);

	যদি (BOARD_IS_BBXM(machine.board_id)) {
		tsc_per_ms[0] = 16250;
	} অন্যথা যদি (BOARD_IS_BB(machine.board_id)) {
		tsc_per_ms[0] = 15000;
	} অন্যথা {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_per_tick[0] = tsc_per_ms[0] * 1000 / system_hz;

	ফেরত 0;
}

শূন্য stop_local_timer(শূন্য)
{
	bsp_timer_stop();
}

শূন্য arch_timer_int_handler(শূন্য)
{
	bsp_timer_int_handler();
}

শূন্য cycles_accounting_init(শূন্য)
{
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_last_tsc) = 0;
	get_cpu_var(cpu, cpu_last_idle) = 0;
}

শূন্য context_stop(কাঠামো proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	unsigned পূর্ণসংখ্যা cpu, tpt, গণক;

#ifdef CONFIG_SMP
#error CONFIG_SMP is unsupported on ARM
#অন্যথা
	read_tsc_64(&tsc);
	p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;
	cpu = 0;
#endif

	tsc_delta = tsc - *__tsc_ctr_switch;

	যদি (kbill_ipc) {
		kbill_ipc->p_kipc_cycles += tsc_delta;
		kbill_ipc = ফাঁকা;
	}

	যদি (kbill_kcall) {
		kbill_kcall->p_kcall_cycles += tsc_delta;
		kbill_kcall = ফাঁকা;
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
	যখন (tpt > 0 && p->p_tick_cycles >= tpt) {
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
	যদি (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		যদি (p->p_priv != priv_addr(USER_PRIV_ID))
			গণক = CP_SYS;
		অন্যথা যদি (p->p_misc_flags & MF_NICED)
			গণক = CP_NICE;
		অন্যথা
			গণক = CP_USER;

#যদি DEBUG_RACE
		p->p_cpu_time_বাম = 0;
#অন্যথা
		যদি (tsc_delta < p->p_cpu_time_বাম) {
			p->p_cpu_time_বাম -= tsc_delta;
		} অন্যথা {
			p->p_cpu_time_বাম = 0;
		}
#endif
	} অন্যথা {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		যদি (p->p_endpoint == IDLE)
			গণক = CP_IDLE;
		অন্যথা
			গণক = CP_INTR;
	}

	tsc_per_state[cpu][গণক] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

শূন্য context_stop_idle(শূন্য)
{
	পূর্ণসংখ্যা is_idle;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#endif

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	যদি (is_idle)
		restart_local_timer();
#যদি SPROFILE
	যদি (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#endif
}

শূন্য restart_local_timer(শূন্য)
{
}

পূর্ণসংখ্যা register_local_timer_handler(ধ্রুবক irq_handler_t handler)
{
	ফেরত bsp_register_timer_handler(handler);
}

u64_t ms_2_cpu_time(unsigned ms)
{
	ফেরত (u64_t)tsc_per_ms[cpuid] * ms;
}

unsigned cpu_time_2_ms(u64_t cpu_time)
{
	ফেরত (unsigned long)(cpu_time / tsc_per_ms[cpuid]);
}

short cpu_load(শূন্য)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	কাঠামো proc *idle;
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
	যদি (*last_tsc) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = *current_idle - *last_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		load = ex64lo(busy / tsc_delta);

		যদি (load > 100)
			load = 100;
	} অন্যথা
		load = 0;

	*last_tsc = current_tsc;
	*last_idle = *current_idle;
	ফেরত load;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
শূন্য
get_cpu_ticks(unsigned পূর্ণসংখ্যা cpu, uint64_t ticks[CPUSTATES])
{
	পূর্ণসংখ্যা i;

	/* TODO: make this inter-CPU safe! */
	জন্য (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_per_state[cpu][i] / tsc_per_tick[cpu];
}
