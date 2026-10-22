/* i386-specific clock functions. */

#ضمن <machine/ports.h>

#ضمن "kernel/clock.h"
#ضمن "kernel/interrupt.h"
#ضمن <minix/u64.h>

#ضمن <sys/sched.h> /* for CP_*, CPUSTATES */
#إذا CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#غلطي "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_APIC
#ضمن "apic.h"
#جيڪڏهن_پورو

#ضمن "kernel/spinlock.h"

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
#ضمن "kernel/smp.h"
#جيڪڏهن_پورو

#وصف_ڪريو CLOCK_ACK_ٻٽو_انگ   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#وصف_ڪريو COUNTER_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#وصف_ڪريو LATCH_ڳڻپ     0x00    /* cc00xxxx, c = channel, x = any */
#وصف_ڪريو چورس_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#وصف_ڪريو TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#وصف_ڪريو TIMER_ڳڻپ(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

ساكن irq_hook_t pic_timer_hook;		/* interrupt handler hook */

ساكن بنانشان probe_ticks;
ساكن u64_t tsc0, tsc1;
#وصف_ڪريو PROBE_TICKS	(system_hz / 10)

ساكن بنانشان tsc_في_ms[CONFIG_MAX_CPUS];
ساكن بنانشان tsc_في_tick[CONFIG_MAX_CPUS];
ساكن uint64_t tsc_في_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
صحيح init_8253A_timer(ثابت بنانشان freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_گهڻائي, چورس_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_ڳڻپ(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_ڳڻپ(freq) >> 8); /* timer high byte */

	أرجع OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
فراغ stop_8253A_timer(فراغ)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_گهڻائي, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

فراغ arch_timer_int_handler(فراغ)
{
}

ساكن صحيح calib_cpu_handler(irq_hook_t * اڻ_استعمال_ٿيل(hook))
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
	BKL_تالو();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_سيٽ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (بنانشان ڊگهو)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

صحيح init_local_timer(بنانشان freq)
{
#جيڪڏهن_وصف_ٿيل USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	إذا (lapic_addr) {
		بنانشان cpu = cpuid;
		tsc_في_ms[cpu] = (بنانشان)(cpu_get_freq(cpu) / 1000);
		tsc_في_tick[cpu] = (بنانشان)(cpu_get_freq(cpu) / system_hz);
		lapic_سيٽ_timer_one_shot(1000000 / system_hz);
	} وإلا {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#وإلا
	{
#جيڪڏهن_پورو
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_في_ms[0] = (بنانشان ڊگهو)(cpu_get_freq(0) / 1000);
		tsc_في_tick[0] = (بنانشان)(cpu_get_freq(0) / system_hz);
	}

	أرجع 0;
}

فراغ stop_local_timer(فراغ)
{
#جيڪڏهن_وصف_ٿيل USE_APIC
	إذا (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} وإلا
#جيڪڏهن_پورو
	{
		stop_8253A_timer();
	}
}

فراغ restart_local_timer(فراغ)
{
#جيڪڏهن_وصف_ٿيل USE_APIC
	إذا (lapic_addr) {
		lapic_restart_timer();
	}
#جيڪڏهن_پورو
}

صحيح رجسٽر_local_timer_handler(ثابت irq_handler_t handler)
{
#جيڪڏهن_وصف_ٿيل USE_APIC
	إذا (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(لکو("Using LAPIC timer as tick source\n"));
	} وإلا
#جيڪڏهن_پورو
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	أرجع 0;
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
		p->p_چڪرن = p->p_چڪرن + tmp;
		must_bkl_unlock = 1;
	} وإلا {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_تالو.val;
		
		BKL_تالو();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_چڪرن = p->p_چڪرن + tsc - *__tsc_ctr_switch;

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
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
#جيڪڏهن_پورو
	}
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

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
	إذا(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#جيڪڏهن_پورو
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

فراغ busy_delay_ms(صحيح ms)
{
	u64_t چڪرن = ms_2_cpu_وقت(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + چڪرن;
	افعل { read_tsc_64(&tsc); } طالما(tsc < tsc1);
	أرجع;
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
