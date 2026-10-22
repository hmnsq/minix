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
#दोषः "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#यदि_अन्तः

#अन्तर्भाव "kernel/spinlock.h"

#यदि_परिभाषितम् CONFIG_SMP
#अन्तर्भाव "kernel/smp.h"
#दोषः CONFIG_SMP is unsupported on ARM
#यदि_अन्तः

#अन्तर्भाव "bsp_timer.h"
#अन्तर्भाव "bsp_intr.h"

स्थिर अचिह्नित tsc_प्रत्येकम्_ms[CONFIG_MAX_CPUS];
स्थिर अचिह्नित tsc_प्रत्येकम्_tick[CONFIG_MAX_CPUS];
स्थिर uint64_t tsc_प्रत्येकम्_state[CONFIG_MAX_CPUS][CPUSTATES];

पूर्णाङ्क init_local_timer(अचिह्नित freq)
{
	bsp_timer_init(freq);

	यदि (BOARD_IS_BBXM(machine.board_id)) {
		tsc_प्रत्येकम्_ms[0] = 16250;
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		tsc_प्रत्येकम्_ms[0] = 15000;
	} अन्यथा {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_प्रत्येकम्_tick[0] = tsc_प्रत्येकम्_ms[0] * 1000 / system_hz;

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

शून्य आवृत्तयः_accounting_init(शून्य)
{
#यदि_परिभाषितम् CONFIG_SMP
	अचिह्नित cpu = cpuid;
#यदि_अन्तः

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_चरमः_tsc) = 0;
	get_cpu_var(cpu, cpu_चरमः_idle) = 0;
}

शून्य context_stop(संरचना proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	अचिह्नित पूर्णाङ्क cpu, tpt, गणक;

#यदि_परिभाषितम् CONFIG_SMP
#दोषः CONFIG_SMP is unsupported on ARM
#अन्यथा
	read_tsc_64(&tsc);
	p->p_आवृत्तयः = p->p_आवृत्तयः + tsc - *__tsc_ctr_switch;
	cpu = 0;
#यदि_अन्तः

	tsc_delta = tsc - *__tsc_ctr_switch;

	यदि (kbill_ipc) {
		kbill_ipc->p_kipc_आवृत्तयः += tsc_delta;
		kbill_ipc = रिक्त;
	}

	यदि (kbill_kcall) {
		kbill_kcall->p_kcall_आवृत्तयः += tsc_delta;
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
	tpt = tsc_प्रत्येकम्_tick[cpu];

	p->p_tick_आवृत्तयः += tsc_delta;
	यावत् (tpt > 0 && p->p_tick_आवृत्तयः >= tpt) {
		p->p_tick_आवृत्तयः -= tpt;

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
			गणक = CP_SYS;
		अन्यथा यदि (p->p_misc_flags & MF_NICED)
			गणक = CP_NICE;
		अन्यथा
			गणक = CP_USER;

#यदि DEBUG_RACE
		p->p_cpu_time_वामः = 0;
#अन्यथा
		यदि (tsc_delta < p->p_cpu_time_वामः) {
			p->p_cpu_time_वामः -= tsc_delta;
		} अन्यथा {
			p->p_cpu_time_वामः = 0;
		}
#यदि_अन्तः
	} अन्यथा {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		यदि (p->p_endpoint == IDLE)
			गणक = CP_IDLE;
		अन्यथा
			गणक = CP_INTR;
	}

	tsc_प्रत्येकम्_state[cpu][गणक] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

शून्य context_stop_idle(शून्य)
{
	पूर्णाङ्क is_idle;
#यदि_परिभाषितम् CONFIG_SMP
	अचिह्नित cpu = cpuid;
#यदि_अन्तः

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	यदि (is_idle)
		restart_local_timer();
#यदि SPROFILE
	यदि (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#यदि_अन्तः
}

शून्य restart_local_timer(शून्य)
{
}

पूर्णाङ्क पञ्जिकाकोशः_local_timer_handler(ध्रुव irq_handler_t handler)
{
	प्रत्यागम bsp_पञ्जिकाकोशः_timer_handler(handler);
}

u64_t ms_2_cpu_time(अचिह्नित ms)
{
	प्रत्यागम (u64_t)tsc_प्रत्येकम्_ms[cpuid] * ms;
}

अचिह्नित cpu_time_2_ms(u64_t cpu_time)
{
	प्रत्यागम (अचिह्नित दीर्घ)(cpu_time / tsc_प्रत्येकम्_ms[cpuid]);
}

ह्रस्व cpu_आरोपय(शून्य)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	संरचना proc *idle;
	ह्रस्व आरोपय;
#यदि_परिभाषितम् CONFIG_SMP
	अचिह्नित cpu = cpuid;
#यदि_अन्तः

	u64_t *चरमः_tsc, *चरमः_idle;

	चरमः_tsc = get_cpu_var_ptr(cpu, cpu_चरमः_tsc);
	चरमः_idle = get_cpu_var_ptr(cpu, cpu_चरमः_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_आवृत्तयः; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	यदि (*चरमः_tsc) {
		tsc_delta = current_tsc - *चरमः_tsc;
		idle_delta = *current_idle - *चरमः_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		आरोपय = ex64lo(busy / tsc_delta);

		यदि (आरोपय > 100)
			आरोपय = 100;
	} अन्यथा
		आरोपय = 0;

	*चरमः_tsc = current_tsc;
	*चरमः_idle = *current_idle;
	प्रत्यागम आरोपय;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
शून्य
get_cpu_ticks(अचिह्नित पूर्णाङ्क cpu, uint64_t ticks[CPUSTATES])
{
	पूर्णाङ्क i;

	/* TODO: make this inter-CPU safe! */
	प्रति (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_प्रत्येकम्_state[cpu][i] / tsc_प्रत्येकम्_tick[cpu];
}
