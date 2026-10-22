/* ARM-specific clock functions. */

#شامل "kernel/kernel.h"

#شامل "kernel/clock.h"
#شامل "kernel/interrupt.h"
#شامل <minix/u64.h>
#شامل <minix/board.h>
#شامل "kernel/glo.h"
#شامل "kernel/profile.h"

#شامل <sys/sched.h> /* for CP_*, CPUSTATES */
#اگر CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#غلطی "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#اگر_ختم

#شامل "kernel/spinlock.h"

#اگر_تعریف_شدہ CONFIG_SMP
#شامل "kernel/smp.h"
#غلطی CONFIG_SMP is unsupported on ARM
#اگر_ختم

#شامل "bsp_timer.h"
#شامل "bsp_intr.h"

ساکن غیرعلامتی tsc_فی_ms[CONFIG_MAX_CPUS];
ساکن غیرعلامتی tsc_فی_tick[CONFIG_MAX_CPUS];
ساکن uint64_t tsc_فی_state[CONFIG_MAX_CPUS][CPUSTATES];

صحیح init_local_timer(غیرعلامتی freq)
{
	bsp_timer_init(freq);

	اگر (BOARD_IS_BBXM(machine.board_id)) {
		tsc_فی_ms[0] = 16250;
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		tsc_فی_ms[0] = 15000;
	} ورنہ {
		panic("Can not do the clock setup. machine (0x%08x) is unknown\n",machine.board_id);
	};

	tsc_فی_tick[0] = tsc_فی_ms[0] * 1000 / system_hz;

	واپس 0;
}

خالی stop_local_timer(خالی)
{
	bsp_timer_stop();
}

خالی arch_timer_int_handler(خالی)
{
	bsp_timer_int_handler();
}

خالی ادوار_accounting_init(خالی)
{
#اگر_تعریف_شدہ CONFIG_SMP
	غیرعلامتی cpu = cpuid;
#اگر_ختم

	read_tsc_64(get_cpu_var_ptr(cpu, tsc_ctr_switch));

	get_cpu_var(cpu, cpu_آخری_tsc) = 0;
	get_cpu_var(cpu, cpu_آخری_idle) = 0;
}

خالی context_stop(ساخت proc * p)
{
	u64_t tsc, tsc_delta;
	u64_t * __tsc_ctr_switch = get_cpulocal_var_ptr(tsc_ctr_switch);
	غیرعلامتی صحیح cpu, tpt, شمار_2;

#اگر_تعریف_شدہ CONFIG_SMP
#غلطی CONFIG_SMP is unsupported on ARM
#ورنہ
	read_tsc_64(&tsc);
	p->p_ادوار = p->p_ادوار + tsc - *__tsc_ctr_switch;
	cpu = 0;
#اگر_ختم

	tsc_delta = tsc - *__tsc_ctr_switch;

	اگر (kbill_ipc) {
		kbill_ipc->p_kipc_ادوار += tsc_delta;
		kbill_ipc = تہی;
	}

	اگر (kbill_kcall) {
		kbill_kcall->p_kcall_ادوار += tsc_delta;
		kbill_kcall = تہی;
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

	p->p_tick_ادوار += tsc_delta;
	جب (tpt > 0 && p->p_tick_ادوار >= tpt) {
		p->p_tick_ادوار -= tpt;

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
	اگر (p->p_endpoint >= 0) {
		/* On MINIX3, the "system" counter covers system processes. */
		اگر (p->p_priv != priv_addr(USER_PRIV_ID))
			شمار_2 = CP_SYS;
		ورنہ اگر (p->p_misc_flags & MF_NICED)
			شمار_2 = CP_NICE;
		ورنہ
			شمار_2 = CP_USER;

#اگر DEBUG_RACE
		p->p_cpu_وقت_بایاں = 0;
#ورنہ
		اگر (tsc_delta < p->p_cpu_وقت_بایاں) {
			p->p_cpu_وقت_بایاں -= tsc_delta;
		} ورنہ {
			p->p_cpu_وقت_بایاں = 0;
		}
#اگر_ختم
	} ورنہ {
		/* On MINIX3, the "interrupts" counter covers the kernel. */
		اگر (p->p_endpoint == IDLE)
			شمار_2 = CP_IDLE;
		ورنہ
			شمار_2 = CP_INTR;
	}

	tsc_فی_state[cpu][شمار_2] += tsc_delta;

	*__tsc_ctr_switch = tsc;
}

خالی context_stop_idle(خالی)
{
	صحیح is_idle;
#اگر_تعریف_شدہ CONFIG_SMP
	غیرعلامتی cpu = cpuid;
#اگر_ختم

	is_idle = get_cpu_var(cpu, cpu_is_idle);
	get_cpu_var(cpu, cpu_is_idle) = 0;

	context_stop(get_cpulocal_var_ptr(idle_proc));

	اگر (is_idle)
		restart_local_timer();
#اگر SPROFILE
	اگر (sprofiling)
		get_cpulocal_var(idle_interrupted) = 1;
#اگر_ختم
}

خالی restart_local_timer(خالی)
{
}

صحیح رجسٹر_local_timer_handler(ثابت irq_handler_t handler)
{
	واپس bsp_رجسٹر_timer_handler(handler);
}

u64_t ms_2_cpu_وقت(غیرعلامتی ms)
{
	واپس (u64_t)tsc_فی_ms[cpuid] * ms;
}

غیرعلامتی cpu_وقت_2_ms(u64_t cpu_وقت)
{
	واپس (غیرعلامتی طویل)(cpu_وقت / tsc_فی_ms[cpuid]);
}

مختصر cpu_بھرو(خالی)
{
	u64_t current_tsc, *current_idle;
	u64_t tsc_delta, idle_delta, busy;
	ساخت proc *idle;
	مختصر بھرو;
#اگر_تعریف_شدہ CONFIG_SMP
	غیرعلامتی cpu = cpuid;
#اگر_ختم

	u64_t *آخری_tsc, *آخری_idle;

	آخری_tsc = get_cpu_var_ptr(cpu, cpu_آخری_tsc);
	آخری_idle = get_cpu_var_ptr(cpu, cpu_آخری_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);;
	read_tsc_64(&current_tsc);
	current_idle = &idle->p_ادوار; /* ptr to idle proc */

	/* calculate load since last cpu_load invocation */
	اگر (*آخری_tsc) {
		tsc_delta = current_tsc - *آخری_tsc;
		idle_delta = *current_idle - *آخری_idle;

		busy = tsc_delta - idle_delta;
		busy = busy * 100;
		بھرو = ex64lo(busy / tsc_delta);

		اگر (بھرو > 100)
			بھرو = 100;
	} ورنہ
		بھرو = 0;

	*آخری_tsc = current_tsc;
	*آخری_idle = *current_idle;
	واپس بھرو;
}

/*
 * Return the number of clock ticks spent in each of a predefined number of
 * CPU states.
 */
خالی
get_cpu_ticks(غیرعلامتی صحیح cpu, uint64_t ticks[CPUSTATES])
{
	صحیح i;

	/* TODO: make this inter-CPU safe! */
	برائے (i = 0; i < CPUSTATES; i++)
		ticks[i] = tsc_فی_state[cpu][i] / tsc_فی_tick[cpu];
}
