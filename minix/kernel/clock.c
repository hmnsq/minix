/* This file contains the architecture-independent clock functionality, which
 * handles time related functions.  Important events that are handled here
 * include setting and monitoring alarm timers and deciding when to
 * (re)schedule processes.  System services can access its services through
 * system calls, such as sys_setalarm().
 *
 * Changes:
 *   Aug 18, 2006   removed direct hardware access etc, MinixPPC (Ingmar Alting)
 *   Oct 08, 2005   reordering and comment editing (A. S. Woodhull)
 *   Mar 18, 2004   clock interface moved to SYSTEM task (Jorrit N. Herder)
 *   Sep 30, 2004   source code documentation updated  (Jorrit N. Herder)
 *   Sep 24, 2004   redesigned alarm timers  (Jorrit N. Herder)
 */

#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ "clock.h"

#ifdef USE_WATCHDOG
#ଅନ୍ତର୍ଭାଵ "watchdog.h"
#endif

/* Function prototype for PRIVATE functions.
 */
ସ୍ଥିର ଶୂନ୍ୟ load_update(ଶୂନ୍ୟ);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
ସ୍ଥିର minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
ସ୍ଥିର int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
ଶୂନ୍ୟ
init_clock(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର *ମୂଲ୍ୟ;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, ଆକାର(kclockinfo));

	/* Get clock tick frequency. */
	ମୂଲ୍ୟ = env_get("hz");
	ଯଦି (ମୂଲ୍ୟ != ଖାଲି)
		kclockinfo.hz = atoi(ମୂଲ୍ୟ);
	ଯଦି (ମୂଲ୍ୟ == ଖାଲି || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, ଆକାର(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା timer_int_handler(ଶୂନ୍ୟ)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	ସଂରଚନା proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#ifdef USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#endif

	ଯଦି (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		ଯଦି (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} ଅନ୍ୟଥା {
			kclockinfo.realtime++;
		}
	}

	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	p = get_cpulocal_var(proc_ptr);
	billp = get_cpulocal_var(bill_ptr);

	p->p_user_time++;

	ଯଦି (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	ଯଦି ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_ବାମ > 0)) {
		p->p_virt_ବାମ--;
	}
	ଯଦି ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_ବାମ > 0)) {
		p->p_prof_ବାମ--;
	}
	ଯଦି (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_ବାମ > 0)) {
		billp->p_prof_ବାମ--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	ଯଦି (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	ଯଦି (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		ଯଦି (clock_timers != ଖାଲି &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, ଖାଲି);

#ifdef DEBUG_SERIAL
		ଯଦି (kinfo.do_serial_debug)
			do_ser_debug();
#endif

	}

	arch_timer_int_handler();

	ଫେରନ୍ତୁ(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(ଶୂନ୍ୟ)
{
  /* Get and return the current wall time in ticks since boot. */
  ଫେରନ୍ତୁ(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
ଶୂନ୍ୟ set_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
ଶୂନ୍ୟ set_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(ଶୂନ୍ୟ)
{
  /* Get and return the number of ticks since boot. */
  ଫେରନ୍ତୁ(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
ଶୂନ୍ୟ set_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(ଶୂନ୍ୟ)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  ଫେରନ୍ତୁ(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
ଶୂନ୍ୟ set_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_time,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (ଶୂନ୍ୟ)tmrs_settimer(&clock_timers, tp, exp_time, watchdog, arg, ଖାଲି, ଖାଲି);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
ଶୂନ୍ୟ reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  ଯଦି (tmr_is_set(tp))
	(ଶୂନ୍ୟ)tmrs_clrtimer(&clock_timers, tp, ଖାଲି, ଖାଲି);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ load_update(ଶୂନ୍ୟ)
{
	u16_t slot;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା enqueued = 0, q;
	ସଂରଚନା proc *p;
	ସଂରଚନା proc **rdy_head;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (kclockinfo.uptime / system_hz / _LOAD_UNIT_SECS) %
	    _LOAD_HISTORY;
	ଯଦି(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	rdy_head = get_cpulocal_var(run_q_head);
	/* Cumulation. How many processes are ready now? */
	ପାଇଁ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ପାଇଁ(p = rdy_head[q]; p != ଖାଲି; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = kclockinfo.uptime;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା boot_cpu_init_timer(unsigned freq)
{
	ଯଦି (init_local_timer(freq))
		ଫେରନ୍ତୁ -1;

	ଯଦି (register_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା app_cpu_init_timer(unsigned freq)
{
	ଯଦି (init_local_timer(freq))
		ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
}
