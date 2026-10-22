/* ARM-specific clock functions. */

#ضمن "kernel/kernel.h"

#ضمن "kernel/clock.h"
#ضمن "kernel/interrupt.h"
#ضمن <minix/u64.h>
#ضمن <minix/board.h>
#ضمن "kernel/glo.h"
#ضمن "kernel/profile.h"

#ضمن <sys/sched.h> /* for CP_*, CPUSTATES */
#إذا CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#غلطی "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#جے_انت

#ضمن "kernel/spinlock.h"

#جے_تعریف_شدہ CONFIG_SMP
#ضمن "kernel/smp.h"
#غلطی CONFIG_SMP is unsupported on ARM
#جے_انت

#ضمن "bsp_timer.h"
#ضمن "bsp_intr.h"

ساكن بنانشانی tsc_فی_ms[CONFIG_MAX_CPUS];
ساكن بنانشانی tsc_فی_tick[CONFIG_MAX_CPUS];
ساكن uint64_t tsc_فی_state[CONFIG_MAX_CPUS][CPUSTATES];

صحيح init_local_timer(بنانشانی freq)
{
	bsp_timer_init(freq);

	إذا (BOARD_IS_BBXM(machine.board_id)) {
		tsc_فی_ms[0] = 16250;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		tsc_فی_ms[0] = 15000;
	} وإلا {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_فی_tick[0] = tsc_فی_ms[0] * 1000 / system_hz;

	أرجع 0;
}

فراغ stop_local_timer(فراغ)
{
	bsp_timer_stop();
}

فراغ arch_timer_int_handler(فراغ)
{
	bsp_timer_int_handler();
}

فراغ چکراں_accounting_init(فراغ)
{
#جے_تعریف_شدہ CONFIG_SMP
	بنانشانی cpu = cpuid;
#جے_انت

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_آخری_tsc) = 0;
	get_cpu_var(cpu, cpu_آخری_idle) = 0;
}

فراغ context_stop(بنية proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	بنانشانی صحيح cpu, tpt, counter;

#جے_تعریف_شدہ CONFIG_SMP
#غلطی CONFIG_SMP is unsupported on ARM
#وإلا
	read_tsc_64(&tsc);
	p->p_چکراں = p->p_چکراں + tsc - *__tsc_ctr_switch;
	cpu = 0;
#جے_انت

	tsc_delta = tsc - *__tsc_ctr_switch;

	إذا (kbill_ipc) {
		kbill_ipc->p_kipc_چکراں += tsc_delta;
		kbill_ipc = عدم;
	}

	إذا (kbill_kcall) {
		kbill_kcall->p_kcall_چکراں += tsc_delta;
		kbill_kcall = عدم;
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
	tpt = tsc_فی_tick[cpu];

	p->p_tick_چکراں += tsc_delta;
	طالما (tpt > 0 && p->p_tick_چکراں >= tpt) {
		p->p_tick_چکراں -= tpt;

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
	إذا (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		إذا (p->p_priv != priv_addr(USER_PRIV_ID))
			counter = CP_SYS;
		وإلا إذا (p->p_misc_flags & MF_NICED)
			counter = CP_NICE;
		وإلا
			counter = CP_USER;

#إذا DEBUG_RACE
		p->p_cpu_ویلا_بایاں = 0;
#وإلا
		إذا (tsc_delta < p->p_cpu_ویلا_بایاں) {
			p->p_cpu_ویلا_بایاں -= tsc_delta;
		} وإلا {
			p->p_cpu_ویلا_بایاں = 0;
		}
#جے_انت
	} وإلا {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		إذا (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		وإلا
			counter = CP_INTR;
	}

	tsc_فی_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

فراغ context_stop_idle(فراغ)
{
	صحيح is_idle;
#جے_تعریف_شدہ CONFIG_SMP
	بنانشانی cpu = cpuid;
#جے_انت

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	إذا (is_idle)
		restart_local_timer();
#إذا SPROFILE
	إذا (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#جے_انت
}

فراغ restart_local_timer(فراغ)
{
}

صحيح رجسٹر_local_timer_handler(ثابت irq_handler_t handler)
{
	أرجع bsp_رجسٹر_timer_handler(handler);
}

u64_t ms_2_cpu_ویلا(بنانشانی ms)
{
	أرجع (u64_t)tsc_فی_ms[cpuid] * ms;
}

بنانشانی cpu_ویلا_2_ms(u64_t cpu_ویلا)
{
	أرجع (بنانشانی لما)(cpu_ویلا / tsc_فی_ms[cpuid]);
}

چھوٹا cpu_لدو(فراغ)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	بنية proc *idle;
	چھوٹا لدو;
#جے_تعریف_شدہ CONFIG_SMP
	بنانشانی cpu = cpuid;
#جے_انت

	u64_t *آخری_tsc, *آخری_idle;

	آخری_tsc = get_cpu_var_ptr(cpu, cpu_آخری_tsc);
	آخری_idle = get_cpu_var_ptr(cpu, cpu_آخری_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_چکراں; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	إذا (*آخری_tsc) {
		tsc_delta = current_tsc - *آخری_tsc;
		idle_delta = *current_idle - *آخری_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		لدو = ex64lo(busy / tsc_delta);

		إذا (لدو > 100)
			لدو = 100;
	} وإلا
		لدو = 0;

	*آخری_tsc = current_tsc;
	*آخری_idle = *current_idle;
	أرجع لدو;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
فراغ
get_cpu_ticks(بنانشانی صحيح cpu, uint64_t ticks[CPUSTATES])
{
	صحيح i;

	/* TODO: make this inter-CPU safe! */
	لكل (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_فی_state[cpu][i] / tsc_فی_tick[cpu];
}
