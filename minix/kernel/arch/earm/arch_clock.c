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
#غلطي "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#جيڪڏهن_پورو

#ضمن "kernel/spinlock.h"

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
#ضمن "kernel/smp.h"
#غلطي CONFIG_SMP is unsupported on ARM
#جيڪڏهن_پورو

#ضمن "bsp_timer.h"
#ضمن "bsp_intr.h"

ساكن بنانشان tsc_في_ms[CONFIG_MAX_CPUS];
ساكن بنانشان tsc_في_tick[CONFIG_MAX_CPUS];
ساكن uint64_t tsc_في_state[CONFIG_MAX_CPUS][CPUSTATES];

صحيح init_local_timer(بنانشان freq)
{
	bsp_timer_init(freq);

	إذا (BOARD_IS_BBXM(machine.board_id)) {
		tsc_في_ms[0] = 16250;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		tsc_في_ms[0] = 15000;
	} وإلا {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_في_tick[0] = tsc_في_ms[0] * 1000 / system_hz;

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

فراغ چڪرن_accounting_init(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
	بنانشان cpu = cpuid;
#جيڪڏهن_پورو

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_آخري_tsc) = 0;
	get_cpu_var(cpu, cpu_آخري_idle) = 0;
}

فراغ context_stop(بنية proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	بنانشان صحيح cpu, tpt, counter;

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
#غلطي CONFIG_SMP is unsupported on ARM
#وإلا
	read_tsc_64(&tsc);
	p->p_چڪرن = p->p_چڪرن + tsc - *__tsc_ctr_switch;
	cpu = 0;
#جيڪڏهن_پورو

	tsc_delta = tsc - *__tsc_ctr_switch;

	إذا (kbill_ipc) {
		kbill_ipc->p_kipc_چڪرن += tsc_delta;
		kbill_ipc = عدم;
	}

	إذا (kbill_kcall) {
		kbill_kcall->p_kcall_چڪرن += tsc_delta;
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
	tpt = tsc_في_tick[cpu];

	p->p_tick_چڪرن += tsc_delta;
	طالما (tpt > 0 && p->p_tick_چڪرن >= tpt) {
		p->p_tick_چڪرن -= tpt;

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
		p->p_cpu_وقت_کاٻو = 0;
#وإلا
		إذا (tsc_delta < p->p_cpu_وقت_کاٻو) {
			p->p_cpu_وقت_کاٻو -= tsc_delta;
		} وإلا {
			p->p_cpu_وقت_کاٻو = 0;
		}
#جيڪڏهن_پورو
	} وإلا {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		إذا (p->p_endpoint == IDLE)
			counter = CP_IDLE;
		وإلا
			counter = CP_INTR;
	}

	tsc_في_state[cpu][counter] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

فراغ context_stop_idle(فراغ)
{
	صحيح is_idle;
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
	بنانشان cpu = cpuid;
#جيڪڏهن_پورو

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	إذا (is_idle)
		restart_local_timer();
#إذا SPROFILE
	إذا (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#جيڪڏهن_پورو
}

فراغ restart_local_timer(فراغ)
{
}

صحيح رجسٽر_local_timer_handler(ثابت irq_handler_t handler)
{
	أرجع bsp_رجسٽر_timer_handler(handler);
}

u64_t ms_2_cpu_وقت(بنانشان ms)
{
	أرجع (u64_t)tsc_في_ms[cpuid] * ms;
}

بنانشان cpu_وقت_2_ms(u64_t cpu_وقت)
{
	أرجع (بنانشان ڊگهو)(cpu_وقت / tsc_في_ms[cpuid]);
}

ننڍو cpu_ڀريو(فراغ)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	بنية proc *idle;
	ننڍو ڀريو;
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
	بنانشان cpu = cpuid;
#جيڪڏهن_پورو

	u64_t *آخري_tsc, *آخري_idle;

	آخري_tsc = get_cpu_var_ptr(cpu, cpu_آخري_tsc);
	آخري_idle = get_cpu_var_ptr(cpu, cpu_آخري_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_چڪرن; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	إذا (*آخري_tsc) {
		tsc_delta = current_tsc - *آخري_tsc;
		idle_delta = *current_idle - *آخري_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		ڀريو = ex64lo(busy / tsc_delta);

		إذا (ڀريو > 100)
			ڀريو = 100;
	} وإلا
		ڀريو = 0;

	*آخري_tsc = current_tsc;
	*آخري_idle = *current_idle;
	أرجع ڀريو;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
فراغ
get_cpu_ticks(بنانشان صحيح cpu, uint64_t ticks[CPUSTATES])
{
	صحيح i;

	/* TODO: make this inter-CPU safe! */
	لكل (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_في_state[cpu][i] / tsc_في_tick[cpu];
}
