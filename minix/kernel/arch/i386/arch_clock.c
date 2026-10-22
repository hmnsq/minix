/* i386-specific clock functions. */

#ضمن <machine/ports.h>

#ضمن "kernel/clock.h"
#ضمن "kernel/interrupt.h"
#ضمن <minix/u64.h>

#ضمن <sys/sched.h> /* for CP_*, CPUSTATES */
#إذا CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#غلطی "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#جے_انت

#جے_تعریف_شدہ USE_APIC
#ضمن "apic.h"
#جے_انت

#ضمن "kernel/spinlock.h"

#جے_تعریف_شدہ CONFIG_SMP
#ضمن "kernel/smp.h"
#جے_انت

#تعریف_کرو CLOCK_ACK_ثنائی_ہندسہ   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#تعریف_کرو COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#تعریف_کرو LATCH_گنتی     0x00    /* cc00xxxx, c = channel, x = any */
#تعریف_کرو مربع_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#تعریف_کرو TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#تعریف_کرو TIMER_گنتی(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ساكن irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ساكن بنانشانی probe_ticks;
ساكن u64_t tsc0, tsc1;
#تعریف_کرو PROBE_TICKS	(system_hz / 10)

ساكن بنانشانی tsc_فی_ms[CONFIG_MAX_CPUS];
ساكن بنانشانی tsc_فی_tick[CONFIG_MAX_CPUS];
ساكن uint64_t tsc_فی_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
صحيح init_8253A_timer(ثابت بنانشانی freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_منوال, مربع_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_گنتی(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_گنتی(freq) >> 8); /* timer high byte */

	أرجع OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
فراغ stop_8253A_timer(فراغ)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_منوال, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

فراغ arch_timer_int_handler(فراغ)
{
}

ساكن صحيح calib_cpu_handler(irq_hook_t * بن_ورتیا(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	إذا (probe_ticks == 1) {
		tsc0 = tsc;
	}
	وإلا إذا (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	أرجع 1;
}

ساكن فراغ estimate_cpu_freq(فراغ)
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
	طالما(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_جندرا();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_سیٹ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (بنانشانی لما)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

صحيح init_local_timer(بنانشانی freq)
{
#جے_تعریف_شدہ USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	إذا (lapic_addr) {
		بنانشانی cpu = cpuid;
		tsc_فی_ms[cpu] = (بنانشانی)(cpu_get_freq(cpu) / 1000);
		tsc_فی_tick[cpu] = (بنانشانی)(cpu_get_freq(cpu) / system_hz);
		lapic_سیٹ_timer_one_shot(1000000 / system_hz);
	} وإلا {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#وإلا
	{
#جے_انت
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_فی_ms[0] = (بنانشانی لما)(cpu_get_freq(0) / 1000);
		tsc_فی_tick[0] = (بنانشانی)(cpu_get_freq(0) / system_hz);
	}

	أرجع 0;
}

فراغ stop_local_timer(فراغ)
{
#جے_تعریف_شدہ USE_APIC
	إذا (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} وإلا
#جے_انت
	{
		stop_8253A_timer();
	}
}

فراغ restart_local_timer(فراغ)
{
#جے_تعریف_شدہ USE_APIC
	إذا (lapic_addr) {
		lapic_restart_timer();
	}
#جے_انت
}

صحيح رجسٹر_local_timer_handler(ثابت irq_handler_t handler)
{
#جے_تعریف_شدہ USE_APIC
	إذا (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(لکھو("Using LAPIC timer as tick source\n"));
	} وإلا
#جے_انت
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	أرجع 0;
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
	صحيح must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	إذا (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_چکراں = p->p_چکراں + tmp;
		must_bkl_unlock = 1;
	} وإلا {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_جندرا.val;
		
		BKL_جندرا();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_چکراں = p->p_چکراں + tsc - *__tsc_ctr_switch;

#جے_تعریف_شدہ CONFIG_SMP
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
#جے_انت
	}
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

#جے_تعریف_شدہ CONFIG_SMP
	إذا(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#جے_انت
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

فراغ busy_delay_ms(صحيح ms)
{
	u64_t چکراں = ms_2_cpu_ویلا(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + چکراں;
	افعل { read_tsc_64(&tsc); } طالما(tsc < tsc1);
	أرجع;
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
