/* ARM-specific clock functions. */

#समावेश "kernel/kernel.h"

#समावेश "kernel/clock.h"
#समावेश "kernel/interrupt.h"
#समावेश <minix/u64.h>
#समावेश <minix/board.h>
#समावेश "kernel/glo.h"
#समावेश "kernel/profile.h"

#समावेश <sys/sched.h> /* for CP_*, CPUSTATES */
#अगर CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#त्रुटि "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#पूर्ण_अगर

#समावेश "kernel/spinlock.h"

#अगर_घोषित CONFIG_SMP
#समावेश "kernel/smp.h"
#त्रुटि CONFIG_SMP is unsupported on ARM
#पूर्ण_अगर

#समावेश "bsp_timer.h"
#समावेश "bsp_intr.h"

अटल अचिन्हित tsc_per_ms[CONFIG_MAX_CPUS];
अटल अचिन्हित tsc_per_tick[CONFIG_MAX_CPUS];
अटल uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

पूर्णांक init_local_timer(अचिन्हित freq)
{
	bsp_timer_init(freq);

	अगर (BOARD_IS_BBXM(machine.board_id)) {
		tsc_per_ms[0] = 16250;
	} अन्यथा अगर (BOARD_IS_BB(machine.board_id)) {
		tsc_per_ms[0] = 15000;
	} अन्यथा {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_per_tick[0] = tsc_per_ms[0] * 1000 / system_hz;

	वापस 0;
}

व्योम stop_local_timer(व्योम)
{
	bsp_timer_stop();
}

व्योम arch_timer_int_handler(व्योम)
{
	bsp_timer_int_handler();
}

व्योम cycles_accounting_init(व्योम)
{
#अगर_घोषित CONFIG_SMP
	अचिन्हित cpu = cpuid;
#पूर्ण_अगर

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_last_tsc) = 0;
	get_cpu_var(cpu, cpu_last_idle) = 0;
}

व्योम context_stop(काष्ठा proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	अचिन्हित पूर्णांक cpu, tpt, गणक;

#अगर_घोषित CONFIG_SMP
#त्रुटि CONFIG_SMP is unsupported on ARM
#अन्यथा
	read_tsc_64(&tsc);
	p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;
	cpu = 0;
#पूर्ण_अगर

	tsc_delta = tsc - *__tsc_ctr_switch;

	अगर (kbill_ipc) {
		kbill_ipc->p_kipc_cycles += tsc_delta;
		kbill_ipc = शून्य;
	}

	अगर (kbill_kcall) {
		kbill_kcall->p_kcall_cycles += tsc_delta;
		kbill_kcall = शून्य;
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
	जबतक (tpt > 0 && p->p_tick_cycles >= tpt) {
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
	अगर (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		अगर (p->p_priv != priv_addr(USER_PRIV_ID))
			गणक = CP_SYS;
		अन्यथा अगर (p->p_misc_flags & MF_NICED)
			गणक = CP_NICE;
		अन्यथा
			गणक = CP_USER;

#अगर DEBUG_RACE
		p->p_cpu_time_बायाँ = 0;
#अन्यथा
		अगर (tsc_delta < p->p_cpu_time_बायाँ) {
			p->p_cpu_time_बायाँ -= tsc_delta;
		} अन्यथा {
			p->p_cpu_time_बायाँ = 0;
		}
#पूर्ण_अगर
	} अन्यथा {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		अगर (p->p_endpoint == IDLE)
			गणक = CP_IDLE;
		अन्यथा
			गणक = CP_INTR;
	}

	tsc_per_state[cpu][गणक] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

व्योम context_stop_idle(व्योम)
{
	पूर्णांक is_idle;
#अगर_घोषित CONFIG_SMP
	अचिन्हित cpu = cpuid;
#पूर्ण_अगर

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	अगर (is_idle)
		restart_local_timer();
#अगर SPROFILE
	अगर (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#पूर्ण_अगर
}

व्योम restart_local_timer(व्योम)
{
}

पूर्णांक register_local_timer_handler(स्थिर irq_handler_t handler)
{
	वापस bsp_register_timer_handler(handler);
}

u64_t ms_2_cpu_time(अचिन्हित ms)
{
	वापस (u64_t)tsc_per_ms[cpuid] * ms;
}

अचिन्हित cpu_time_2_ms(u64_t cpu_time)
{
	वापस (अचिन्हित दीर्घ)(cpu_time / tsc_per_ms[cpuid]);
}

लघु cpu_load(व्योम)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	काष्ठा proc *idle;
	लघु load;
#अगर_घोषित CONFIG_SMP
	अचिन्हित cpu = cpuid;
#पूर्ण_अगर

	u64_t *last_tsc, *last_idle;

	last_tsc = get_cpu_var_ptr(cpu, cpu_last_tsc);
	last_idle = get_cpu_var_ptr(cpu, cpu_last_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_cycles; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	अगर (*last_tsc) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = *current_idle - *last_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		load = ex64lo(busy / tsc_delta);

		अगर (load > 100)
			load = 100;
	} अन्यथा
		load = 0;

	*last_tsc = current_tsc;
	*last_idle = *current_idle;
	वापस load;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
व्योम
get_cpu_ticks(अचिन्हित पूर्णांक cpu, uint64_t ticks[CPUSTATES])
{
	पूर्णांक i;

	/* TODO: make this inter-CPU safe! */
	क्रम (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_per_state[cpu][i] / tsc_per_tick[cpu];
}
