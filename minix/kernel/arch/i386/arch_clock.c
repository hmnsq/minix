/* i386-specific clock functions. */

#شامل <machine/ports.h>

#شامل "kernel/clock.h"
#شامل "kernel/interrupt.h"
#شامل <minix/u64.h>

#شامل <sys/sched.h> /* for CP_*, CPUSTATES */
#اگر CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#غلطی "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#اگر_ختم

#اگر_تعریف_شدہ USE_APIC
#شامل "apic.h"
#اگر_ختم

#شامل "kernel/spinlock.h"

#اگر_تعریف_شدہ CONFIG_SMP
#شامل "kernel/smp.h"
#اگر_ختم

#تعریف_کرو CLOCK_ACK_ثنائی_ہندسہ   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#تعریف_کرو شمار_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#تعریف_کرو LATCH_گنتی     0x00    /* cc00xxxx, c = channel, x = any */
#تعریف_کرو مربع_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#تعریف_کرو TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#تعریف_کرو TIMER_گنتی(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ساکن irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ساکن غیرعلامتی probe_ticks;
ساکن u64_t tsc0, tsc1;
#تعریف_کرو PROBE_TICKS	(system_hz / 10)

ساکن غیرعلامتی tsc_فی_ms[CONFIG_MAX_CPUS];
ساکن غیرعلامتی tsc_فی_tick[CONFIG_MAX_CPUS];
ساکن uint64_t tsc_فی_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
صحیح init_8253A_timer(ثابت غیرعلامتی freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_منوال, مربع_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_گنتی(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_گنتی(freq) >> 8); /* timer high byte */

	واپس OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
خالی stop_8253A_timer(خالی)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_منوال, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

خالی arch_timer_int_handler(خالی)
{
}

ساکن صحیح calib_cpu_handler(irq_hook_t * غیر_مستعمل(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	اگر (probe_ticks == 1) {
		tsc0 = tsc;
	}
	ورنہ اگر (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	واپس 1;
}

ساکن خالی estimate_cpu_freq(خالی)
{
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_cpu;

	/* set the probe, we use the legacy timer, IRQ 0 */
	put_irq_handler(&calib_cpu, CLOCK_IRQ, calib_cpu_handler);

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	/* set the PIC timer to get some time */
	intr_enable();

	/* loop for some time to get a sample */
	جب(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_تالا();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_سیٹ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (غیرعلامتی طویل)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

صحیح init_local_timer(غیرعلامتی freq)
{
#اگر_تعریف_شدہ USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	اگر (lapic_addr) {
		غیرعلامتی cpu = cpuid;
		tsc_فی_ms[cpu] = (غیرعلامتی)(cpu_get_freq(cpu) / 1000);
		tsc_فی_tick[cpu] = (غیرعلامتی)(cpu_get_freq(cpu) / system_hz);
		lapic_سیٹ_timer_one_shot(1000000 / system_hz);
	} ورنہ {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#ورنہ
	{
#اگر_ختم
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_فی_ms[0] = (غیرعلامتی طویل)(cpu_get_freq(0) / 1000);
		tsc_فی_tick[0] = (غیرعلامتی)(cpu_get_freq(0) / system_hz);
	}

	واپس 0;
}

خالی stop_local_timer(خالی)
{
#اگر_تعریف_شدہ USE_APIC
	اگر (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} ورنہ
#اگر_ختم
	{
		stop_8253A_timer();
	}
}

خالی restart_local_timer(خالی)
{
#اگر_تعریف_شدہ USE_APIC
	اگر (lapic_addr) {
		lapic_restart_timer();
	}
#اگر_ختم
}

صحیح رجسٹر_local_timer_handler(ثابت irq_handler_t handler)
{
#اگر_تعریف_شدہ USE_APIC
	اگر (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(لکھو("Using LAPIC timer as tick source\n"));
	} ورنہ
#اگر_ختم
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	واپس 0;
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
	صحیح must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	اگر (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_ادوار = p->p_ادوار + tmp;
		must_bkl_unlock = 1;
	} ورنہ {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_تالا.val;
		
		BKL_تالا();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_ادوار = p->p_ادوار + tsc - *__tsc_ctr_switch;

#اگر_تعریف_شدہ CONFIG_SMP
		/*
		 * Since at the time we got a scheduling IPI we might have been
		 * waiting for BKL already, we may miss it due to a similar IPI to
		 * the cpu which is already waiting for us to handle its. This
		 * results in a live-lock of these two cpus.
		 *
		 * Therefore we always check if there is one pending and if so,
		 * we handle it straight away so the other cpu can continue and
		 * we do not deadlock.
		 */
		smp_sched_handler();
#اگر_ختم
	}
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

#اگر_تعریف_شدہ CONFIG_SMP
	اگر(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#اگر_ختم
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

خالی busy_delay_ms(صحیح ms)
{
	u64_t ادوار = ms_2_cpu_وقت(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + ادوار;
	کرو { read_tsc_64(&tsc); } جب(tsc < tsc1);
	واپس;
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
