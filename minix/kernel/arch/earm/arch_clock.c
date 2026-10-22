/* ARM-specific clock functions. */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/interrupt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/glo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/profile.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sched.h> /* for CP_*, CPUSTATES */
#ਜੇ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#ਗਲਤੀ "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/spinlock.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/smp.h"
#ਗਲਤੀ CONFIG_SMP is unsupported on ARM
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_timer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_intr.h"

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ tsc_ਪ੍ਰਤੀ_ms[CONFIG_MAX_CPUS];
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ tsc_ਪ੍ਰਤੀ_tick[CONFIG_MAX_CPUS];
ਸਥਿਰ uint64_t tsc_ਪ੍ਰਤੀ_state[CONFIG_MAX_CPUS][CPUSTATES];

ਪੂਰਨ init_local_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	bsp_timer_init(freq);

	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		tsc_ਪ੍ਰਤੀ_ms[0] = 16250;
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		tsc_ਪ੍ਰਤੀ_ms[0] = 15000;
	} ਨਹੀਂਤਾਂ {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_ਪ੍ਰਤੀ_tick[0] = tsc_ਪ੍ਰਤੀ_ms[0] * 1000 / system_hz;

	ਵਾਪਸ 0;
}

ਖਾਲੀ stop_local_timer(ਖਾਲੀ)
{
	bsp_timer_stop();
}

ਖਾਲੀ arch_timer_int_handler(ਖਾਲੀ)
{
	bsp_timer_int_handler();
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
#ਗਲਤੀ CONFIG_SMP is unsupported on ARM
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

ਖਾਲੀ restart_local_timer(ਖਾਲੀ)
{
}

ਪੂਰਨ ਰਜਿਸਟਰ_local_timer_handler(ਅਚਲ irq_handler_t handler)
{
	ਵਾਪਸ bsp_ਰਜਿਸਟਰ_timer_handler(handler);
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
