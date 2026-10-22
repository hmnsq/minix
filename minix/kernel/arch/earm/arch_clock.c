/* ARM-specific clock functions. */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ "kernel/interrupt.h"
#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ "kernel/glo.h"
#ಅನ್ತರ್ಭಾವ "kernel/profile.h"

#ಅನ್ತರ್ಭಾವ <sys/sched.h> /* for CP_*, CPUSTATES */
#ಆದರೆ CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#ದೋಷ "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
#ಅನ್ತರ್ಭಾವ "kernel/smp.h"
#ದೋಷ CONFIG_SMP is unsupported on ARM
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "bsp_timer.h"
#ಅನ್ತರ್ಭಾವ "bsp_intr.h"

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ tsc_ಪ್ರತಿ_ms[CONFIG_MAX_CPUS];
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ tsc_ಪ್ರತಿ_tick[CONFIG_MAX_CPUS];
ಸ್ಥಿರ uint64_t tsc_ಪ್ರತಿ_state[CONFIG_MAX_CPUS][CPUSTATES];

ಪೂರ್ಣಾಂಕ init_local_timer(ಚಿಹ್ನೆರಹಿತ freq)
{
	bsp_timer_init(freq);

	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		tsc_ಪ್ರತಿ_ms[0] = 16250;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		tsc_ಪ್ರತಿ_ms[0] = 15000;
	} ಇಲ್ಲದಿದ್ದರೆ {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_ಪ್ರತಿ_tick[0] = tsc_ಪ್ರತಿ_ms[0] * 1000 / system_hz;

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ stop_local_timer(ಶೂನ್ಯ)
{
	bsp_timer_stop();
}

ಶೂನ್ಯ arch_timer_int_handler(ಶೂನ್ಯ)
{
	bsp_timer_int_handler();
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
#ದೋಷ CONFIG_SMP is unsupported on ARM
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

ಶೂನ್ಯ restart_local_timer(ಶೂನ್ಯ)
{
}

ಪೂರ್ಣಾಂಕ ರಿಜಿಸ್ಟರ್_local_timer_handler(ಸ್ಥಿರಾಂಕ irq_handler_t handler)
{
	ಹಿಂತಿರುಗಿ bsp_ರಿಜಿಸ್ಟರ್_timer_handler(handler);
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
