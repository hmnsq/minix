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

#অন্তর্ভাব <minix/endpoint.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <assert.h>

#অন্তর্ভাব "clock.h"

#ifdef USE_WATCHDOG
#অন্তর্ভাব "watchdog.h"
#endif

/* Function prototype for PRIVATE functions.
 */
স্থির শূন্য load_update(শূন্য);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
স্থির minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
স্থির int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
শূন্য
init_clock(শূন্য)
{
	অক্ষর *মান;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, আকার(kclockinfo));

	/* Get clock tick frequency. */
	মান = env_get("hz");
	যদি (মান != ফাঁকা)
		kclockinfo.hz = atoi(মান);
	যদি (মান == ফাঁকা || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, আকার(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
পূর্ণসংখ্যা timer_int_handler(শূন্য)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	কাঠামো proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#ifdef USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#endif

	যদি (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		যদি (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} অন্যথা {
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

	যদি (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	যদি ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_বাম > 0)) {
		p->p_virt_বাম--;
	}
	যদি ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_বাম > 0)) {
		p->p_prof_বাম--;
	}
	যদি (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_বাম > 0)) {
		billp->p_prof_বাম--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	যদি (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	যদি (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		যদি (clock_timers != ফাঁকা &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, ফাঁকা);

#ifdef DEBUG_SERIAL
		যদি (kinfo.do_serial_debug)
			do_ser_debug();
#endif

	}

	arch_timer_int_handler();

	ফেরত(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(শূন্য)
{
  /* Get and return the current wall time in ticks since boot. */
  ফেরত(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
শূন্য set_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
শূন্য set_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(শূন্য)
{
  /* Get and return the number of ticks since boot. */
  ফেরত(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
শূন্য set_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(শূন্য)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  ফেরত(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
শূন্য set_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_time,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  পূর্ণসংখ্যা arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (শূন্য)tmrs_settimer(&clock_timers, tp, exp_time, watchdog, arg, ফাঁকা, ফাঁকা);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
শূন্য reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  যদি (tmr_is_set(tp))
	(শূন্য)tmrs_clrtimer(&clock_timers, tp, ফাঁকা, ফাঁকা);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
স্থির শূন্য load_update(শূন্য)
{
	u16_t slot;
	পূর্ণসংখ্যা enqueued = 0, q;
	কাঠামো proc *p;
	কাঠামো proc **rdy_শীর্ষ;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (kclockinfo.uptime / system_hz / _LOAD_UNIT_SECS) %
	    _LOAD_HISTORY;
	যদি(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	rdy_শীর্ষ = get_cpulocal_var(run_q_শীর্ষ);
	/* Cumulation. How many processes are ready now? */
	জন্য(q = 0; q < NR_SCHED_QUEUES; q++) {
		জন্য(p = rdy_শীর্ষ[q]; p != ফাঁকা; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = kclockinfo.uptime;
}

পূর্ণসংখ্যা boot_cpu_init_timer(unsigned freq)
{
	যদি (init_local_timer(freq))
		ফেরত -1;

	যদি (register_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		ফেরত -1;

	ফেরত 0;
}

পূর্ণসংখ্যা app_cpu_init_timer(unsigned freq)
{
	যদি (init_local_timer(freq))
		ফেরত -1;

	ফেরত 0;
}
