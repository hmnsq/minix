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

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "clock.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
#ਅਨ੍ਤਰ੍ਭਾਵ "watchdog.h"
#ਜੇ_ਅੰਤ

/* Function prototype for PRIVATE functions.
 */
ਸਥਿਰ ਖਾਲੀ ਲੱਦੋ_update(ਖਾਲੀ);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
ਸਥਿਰ minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
ਸਥਿਰ int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
ਖਾਲੀ
init_clock(ਖਾਲੀ)
{
	ਅੱਖਰ *ਮੁੱਲ;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, ਆਕਾਰ(kclockinfo));

	/* Get clock tick frequency. */
	ਮੁੱਲ = env_get("hz");
	ਜੇ (ਮੁੱਲ != ਨਿਰਾ)
		kclockinfo.hz = atoi(ਮੁੱਲ);
	ਜੇ (ਮੁੱਲ == ਨਿਰਾ || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, ਆਕਾਰ(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
ਪੂਰਨ timer_int_handler(ਖਾਲੀ)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	ਬਣਤਰ proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#ਜੇ_ਅੰਤ

	ਜੇ (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		ਜੇ (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} ਨਹੀਂਤਾਂ {
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

	p->p_user_ਸਮਾਂ++;

	ਜੇ (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_ਸਮਾਂ++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	ਜੇ ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_ਖੱਬਾ > 0)) {
		p->p_virt_ਖੱਬਾ--;
	}
	ਜੇ ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_ਖੱਬਾ > 0)) {
		p->p_prof_ਖੱਬਾ--;
	}
	ਜੇ (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_ਖੱਬਾ > 0)) {
		billp->p_prof_ਖੱਬਾ--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	ਜੇ (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	ਲੱਦੋ_update();

	ਜੇ (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		ਜੇ (clock_timers != ਨਿਰਾ &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIAL
		ਜੇ (kinfo.do_serial_debug)
			do_ser_debug();
#ਜੇ_ਅੰਤ

	}

	arch_timer_int_handler();

	ਵਾਪਸ(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(ਖਾਲੀ)
{
  /* Get and return the current wall time in ticks since boot. */
  ਵਾਪਸ(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
ਖਾਲੀ ਸੈੱਟ_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
ਖਾਲੀ ਸੈੱਟ_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(ਖਾਲੀ)
{
  /* Get and return the number of ticks since boot. */
  ਵਾਪਸ(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
ਖਾਲੀ ਸੈੱਟ_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(ਖਾਲੀ)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  ਵਾਪਸ(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
ਖਾਲੀ ਸੈੱਟ_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_ਸਮਾਂ,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  ਪੂਰਨ arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (ਖਾਲੀ)tmrs_settimer(&clock_timers, tp, exp_ਸਮਾਂ, watchdog, arg, ਨਿਰਾ, ਨਿਰਾ);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
ਖਾਲੀ reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  ਜੇ (tmr_is_ਸੈੱਟ(tp))
	(ਖਾਲੀ)tmrs_clrtimer(&clock_timers, tp, ਨਿਰਾ, ਨਿਰਾ);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ ਲੱਦੋ_update(ਖਾਲੀ)
{
	u16_t ਥਾਂ;
	ਪੂਰਨ enqueued = 0, q;
	ਬਣਤਰ proc *p;
	ਬਣਤਰ proc **rdy_ਸਿਰ;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	ਥਾਂ = (kclockinfo.uptime / system_hz / ਲੱਦੋ_UNIT_SECS) %
	    ਲੱਦੋ_HISTORY;
	ਜੇ(ਥਾਂ != kloadinfo.proc_ਆਖਰੀ_ਥਾਂ) {
		kloadinfo.proc_ਲੱਦੋ_history[ਥਾਂ] = 0;
		kloadinfo.proc_ਆਖਰੀ_ਥਾਂ = ਥਾਂ;
	}

	rdy_ਸਿਰ = get_cpulocal_var(ਲੜੀਵਾਰ_q_ਸਿਰ);
	/* Cumulation. How many processes are ready now? */
	ਲਈ(q = 0; q < NR_SCHED_QUEUES; q++) {
		ਲਈ(p = rdy_ਸਿਰ[q]; p != ਨਿਰਾ; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_ਲੱਦੋ_history[ਥਾਂ] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.ਆਖਰੀ_clock = kclockinfo.uptime;
}

ਪੂਰਨ boot_cpu_init_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	ਜੇ (init_local_timer(freq))
		ਵਾਪਸ -1;

	ਜੇ (ਰਜਿਸਟਰ_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		ਵਾਪਸ -1;

	ਵਾਪਸ 0;
}

ਪੂਰਨ app_cpu_init_timer(ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	ਜੇ (init_local_timer(freq))
		ਵਾਪਸ -1;

	ਵਾਪਸ 0;
}
