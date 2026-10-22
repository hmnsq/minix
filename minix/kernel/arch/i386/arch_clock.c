/* i386-specific clock functions. */

#समावेश <machine/ports.h>

#समावेश "kernel/clock.h"
#समावेश "kernel/interrupt.h"
#समावेश <minix/u64.h>

#समावेश <sys/sched.h> /* for CP_*, CPUSTATES */
#अगर CPUSTATES != MINIX_CPUSTATES
/* If this breaks, the code in this file may have to be adapted accordingly. */
#त्रुटि "MINIX_CPUSTATES value is out of sync with NetBSD's!"
#पूर्ण_अगर

#अगर_घोषित USE_APIC
#समावेश "apic.h"
#पूर्ण_अगर

#समावेश "kernel/spinlock.h"

#अगर_घोषित CONFIG_SMP
#समावेश "kernel/smp.h"
#पूर्ण_अगर

#घोषणा CLOCK_ACK_BIT   0x80    /* PS/2 clock interrupt acknowledge bit */

/* Clock parameters. */
#घोषणा गणक_FREQ (2*TIMER_FREQ) /* counter frequency using square wave */
#घोषणा LATCH_गिनती     0x00    /* cc00xxxx, c = channel, x = any */
#घोषणा SQUARE_WAVE     0x36    /* ccaammmb, a = access, m = mode, b = BCD */
                                /*   11x11, 11 = LSB then MSB, x11 = sq wave */
#घोषणा TIMER_FREQ  1193182    /* clock frequency for timer in PC and AT */
#घोषणा TIMER_गिनती(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

अटल irq_hook_t pic_timer_hook;		/* interrupt handler hook */

अटल अचिन्हित probe_ticks;
अटल u64_t tsc0, tsc1;
#घोषणा PROBE_TICKS	(system_hz / 10)

अटल अचिन्हित tsc_per_ms[CONFIG_MAX_CPUS];
अटल अचिन्हित tsc_per_tick[CONFIG_MAX_CPUS];
अटल uint64_t tsc_per_state[CONFIG_MAX_CPUS][CPUSTATES];

/*===========================================================================*
 *				init_8235A_timer			     *
 *===========================================================================*/
पूर्णांक init_8253A_timer(स्थिर अचिन्हित freq)
{
	/* Initialize channel 0 of the 8253A timer to, e.g., 60 Hz,
	 * and register the CLOCK task's interrupt handler to be run
	 * on every clock tick.
	 */
	outb(TIMER_MODE, SQUARE_WAVE);  /* run continuously */
	outb(TIMER0, (TIMER_गिनती(freq) & 0xff)); /* timer low byte */
	outb(TIMER0, TIMER_गिनती(freq) >> 8); /* timer high byte */

	वापस OK;
}

/*===========================================================================*
 *				stop_8235A_timer			     *
 *===========================================================================*/
व्योम stop_8253A_timer(व्योम)
{
	/* Reset the clock to the BIOS rate. (For rebooting.) */
	outb(TIMER_MODE, 0x36);
	outb(TIMER0, 0);
	outb(TIMER0, 0);
}

व्योम arch_timer_int_handler(व्योम)
{
}

अटल पूर्णांक calib_cpu_handler(irq_hook_t * UNUSED(hook))
{
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);


	अगर (probe_ticks == 1) {
		tsc0 = tsc;
	}
	अन्यथा अगर (probe_ticks == PROBE_TICKS) {
		tsc1 = tsc;
	}

	/* just in case we are in an SMP single cpu fallback mode */
	BKL_UNLOCK();
	वापस 1;
}

अटल व्योम estimate_cpu_freq(व्योम)
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
	जबतक(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	/* just in case we are in an SMP single cpu fallback mode */
	BKL_LOCK();

	/* remove the probe */
	rm_irq_handler(&calib_cpu);

	tsc_delta = tsc1 - tsc0;

	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * system_hz;
	cpu_set_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (अचिन्हित दीर्घ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

पूर्णांक init_local_timer(अचिन्हित freq)
{
#अगर_घोषित USE_APIC
	/* if we know the address, lapic is enabled and we should use it */
	अगर (lapic_addr) {
		अचिन्हित cpu = cpuid;
		tsc_per_ms[cpu] = (अचिन्हित)(cpu_get_freq(cpu) / 1000);
		tsc_per_tick[cpu] = (अचिन्हित)(cpu_get_freq(cpu) / system_hz);
		lapic_set_timer_one_shot(1000000 / system_hz);
	} अन्यथा {
		DEBUGBASIC(("Initiating legacy i8253 timer\n"));
#अन्यथा
	{
#पूर्ण_अगर
		init_8253A_timer(freq);
		estimate_cpu_freq();
		/* always only 1 cpu in the system */
		tsc_per_ms[0] = (अचिन्हित दीर्घ)(cpu_get_freq(0) / 1000);
		tsc_per_tick[0] = (अचिन्हित)(cpu_get_freq(0) / system_hz);
	}

	वापस 0;
}

व्योम stop_local_timer(व्योम)
{
#अगर_घोषित USE_APIC
	अगर (lapic_addr) {
		lapic_stop_timer();
		apic_eoi();
	} अन्यथा
#पूर्ण_अगर
	{
		stop_8253A_timer();
	}
}

व्योम restart_local_timer(व्योम)
{
#अगर_घोषित USE_APIC
	अगर (lapic_addr) {
		lapic_restart_timer();
	}
#पूर्ण_अगर
}

पूर्णांक register_local_timer_handler(स्थिर irq_handler_t handler)
{
#अगर_घोषित USE_APIC
	अगर (lapic_addr) {
		/* Using APIC, it is configured in apic_idt_init() */
		BOOT_VERBOSE(म_लिखो("Using LAPIC timer as tick source\n"));
	} अन्यथा
#पूर्ण_अगर
	{
		/* Using PIC, Initialize the CLOCK's interrupt hook. */
		pic_timer_hook.proc_nr_e = NONE;
		pic_timer_hook.irq = CLOCK_IRQ;

		put_irq_handler(&pic_timer_hook, CLOCK_IRQ, handler);
	}

	वापस 0;
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
	पूर्णांक must_bkl_unlock = 0;

	cpu = cpuid;

	/*
	 * This function is called only if we switch from kernel to user or idle
	 * or back. Therefore this is a perfect location to place the big kernel
	 * lock which will hopefully disappear soon.
	 *
	 * If we stop accounting for KERNEL we must unlock the BKL. If account
	 * for IDLE we must not hold the lock
	 */
	अगर (p == proc_addr(KERNEL)) {
		u64_t tmp;

		read_tsc_64(&tsc);
		tmp = tsc - *__tsc_ctr_switch;
		kernel_ticks[cpu] = kernel_ticks[cpu] + tmp;
		p->p_cycles = p->p_cycles + tmp;
		must_bkl_unlock = 1;
	} अन्यथा {
		u64_t bkl_tsc;
		atomic_t succ;
		
		read_tsc_64(&bkl_tsc);
		/* this only gives a good estimate */
		succ = big_kernel_lock.val;
		
		BKL_LOCK();
		
		read_tsc_64(&tsc);

		bkl_ticks[cpu] = bkl_ticks[cpu] + tsc - bkl_tsc;
		bkl_tries[cpu]++;
		bkl_succ[cpu] += !(!(succ == 0));

		p->p_cycles = p->p_cycles + tsc - *__tsc_ctr_switch;

#अगर_घोषित CONFIG_SMP
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
#पूर्ण_अगर
	}
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

#अगर_घोषित CONFIG_SMP
	अगर(must_bkl_unlock) {
		BKL_UNLOCK();
	}
#पूर्ण_अगर
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

व्योम busy_delay_ms(पूर्णांक ms)
{
	u64_t cycles = ms_2_cpu_time(ms), tsc0, tsc, tsc1;
	read_tsc_64(&tsc0);
	tsc1 = tsc0 + cycles;
	करो { read_tsc_64(&tsc); } जबतक(tsc < tsc1);
	वापस;
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
