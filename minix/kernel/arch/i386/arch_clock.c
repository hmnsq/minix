/* i386-specific clock functions. */

#अन्तर्भाव <machine/ports.h>

#अन्तर्भाव "kernel/clock.h"
#अन्तर्भाव "kernel/interrupt.h"
#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव <sys/sched.h> /* for CP_*, CPUSTATES */
#यदि CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#दोषः "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#यदि_अन्तः

#यदि_परिभाषितम् USE_APIC
#अन्तर्भाव "apic.h"
#यदि_अन्तः

#अन्तर्भाव "kernel/spinlock.h"

#यदि_परिभाषितम् CONFIG_SMP
#अन्तर्भाव "kernel/smp.h"
#यदि_अन्तः

#परिभाषय CLOCK_ACK_BIT   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#परिभाषय गणक_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#परिभाषय LATCH_गणनम्     0x00    /* cc00xxxx, c = channel, x = any */
#परिभाषय वर्गफलम्_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#परिभाषय TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#परिभाषय TIMER_गणनम्(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

स्थिर irq_hook_t pic_timer_hook;		/* interrupt handler hook */

स्थिर अचिह्नित probe_ticks;
स्थिर u64_t tsc0, tsc1;
#परिभाषय PROBE_TICKS	(system_hz / 10)

स्थिर अचिह्नित tsc_प्रत्येकम्_ms[CONFIG_MAX_CPUS];
स्थिर अचिह्नित tsc_प्रत्येकम्_tick[CONFIG_MAX_CPUS];
स्थिर uint64_t tsc_प्रत्येकम्_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
पूर्णाङ्क init_8253A_timer(ध्रुव अचिह्नित freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_बहुलकम्, वर्गफलम्_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_गणनम्(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_गणनम्(freq) >> 8); /* timer high byte */

	प्रत्यागम OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
शून्य stop_8253A_timer(शून्य)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_बहुलकम्, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

शून्य arch_timer_int_handler(शून्य)
{
}

स्थिर पूर्णाङ्क calib_cpu_handler(irq_hook_t * अप्रयुक्तम्(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	यदि (probe_ticks == 1) {
		tsc0 = tsc;
	}
	अन्यथा यदि (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	प्रत्यागम 1;
}

स्थिर शून्य estimate_cpu_freq(शून्य)
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
	यावत्(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_तालकम्();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_समुच्चयः_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (अचिह्नित दीर्घ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

पूर्णाङ्क init_local_timer(अचिह्नित freq)
{
#यदि_परिभाषितम् USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	यदि (lapic_addr) {
		अचिह्नित cpu = cpuid;
		tsc_प्रत्येकम्_ms[cpu] = (अचिह्नित)(cpu_get_freq(cpu) / 1000);
		tsc_प्रत्येकम्_tick[cpu] = (अचिह्नित)(cpu_get_freq(cpu) / system_hz);
		lapic_समुच्चयः_timer_one_shot(1000000 / system_hz);
	} अन्यथा {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#अन्यथा
	{
#यदि_अन्तः
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_प्रत्येकम्_ms[0] = (अचिह्नित दीर्घ)(cpu_get_freq(0) / 1000);
		tsc_प्रत्येकम्_tick[0] = (अचिह्नित)(cpu_get_freq(0) / system_hz);
	}

	प्रत्यागम 0;
}

शून्य stop_local_timer(शून्य)
{
#यदि_परिभाषितम् USE_APIC
	यदि (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} अन्यथा
#यदि_अन्तः
	{
		stop_8253A_timer();
	}
}

शून्य restart_local_timer(शून्य)
{
#यदि_परिभाषितम् USE_APIC
	यदि (lapic_addr) {
		lapic_restart_timer();
	}
#यदि_अन्तः
}

पूर्णाङ्क पञ्जिकाकोशः_local_timer_handler(ध्रुव irq_handler_t handler)
{
#यदि_परिभाषितम् USE_APIC
	यदि (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(प्रारूप_लिख("Using LAPIC timer as tick source\n"));
	} अन्यथा
#यदि_अन्तः
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	प्रत्यागम 0;
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
	पूर्णाङ्क must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	यदि (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_आवृत्तयः = p->p_आवृत्तयः + tmp;
		must_bkl_unlock = 1;
	} अन्यथा {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_तालकम्.val;
		
		BKL_तालकम्();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_आवृत्तयः = p->p_आवृत्तयः + tsc - *__tsc_ctr_switch;

#यदि_परिभाषितम् CONFIG_SMP
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
#यदि_अन्तः
	}
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

#यदि_परिभाषितम् CONFIG_SMP
	यदि(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#यदि_अन्तः
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

शून्य busy_delay_ms(पूर्णाङ्क ms)
{
	u64_t आवृत्तयः = ms_2_cpu_time(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + आवृत्तयः;
	कुरु { read_tsc_64(&tsc); } यावत्(tsc < tsc1);
	प्रत्यागम;
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
