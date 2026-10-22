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

#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>

#అన్తర్భావ "clock.h"

#నిర్వచితమైతే USE_WATCHDOG
#అన్తర్భావ "watchdog.h"
#అయితే_ముగింపు

/* Function prototype for PRIVATE functions.
 */
స్థిర శూన్య ఎక్కించు_update(శూన్య);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
స్థిర minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
స్థిర int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
శూన్య
init_clock(శూన్య)
{
	అక్షరం *విలువ;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, పరిమాణం(kclockinfo));

	/* Get clock tick frequency. */
	విలువ = env_get("hz");
	ఒకవేళ (విలువ != ఖాళీ)
		kclockinfo.hz = atoi(విలువ);
	ఒకవేళ (విలువ == ఖాళీ || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, పరిమాణం(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
పూర్ణాంకం timer_int_handler(శూన్య)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	నిర్మాణం proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#నిర్వచితమైతే USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#అయితే_ముగింపు

	ఒకవేళ (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		ఒకవేళ (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} లేకపోతే {
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

	p->p_user_సమయం++;

	ఒకవేళ (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_సమయం++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	ఒకవేళ ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_ఎడమ > 0)) {
		p->p_virt_ఎడమ--;
	}
	ఒకవేళ ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_ఎడమ > 0)) {
		p->p_prof_ఎడమ--;
	}
	ఒకవేళ (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_ఎడమ > 0)) {
		billp->p_prof_ఎడమ--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	ఒకవేళ (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	ఎక్కించు_update();

	ఒకవేళ (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		ఒకవేళ (clock_timers != ఖాళీ &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, ఖాళీ);

#నిర్వచితమైతే DEBUG_SERIAL
		ఒకవేళ (kinfo.do_serial_debug)
			do_ser_debug();
#అయితే_ముగింపు

	}

	arch_timer_int_handler();

	తిరిగి(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(శూన్య)
{
  /* Get and return the current wall time in ticks since boot. */
  తిరిగి(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
శూన్య సమితి_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
శూన్య సమితి_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(శూన్య)
{
  /* Get and return the number of ticks since boot. */
  తిరిగి(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
శూన్య సమితి_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(శూన్య)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  తిరిగి(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
శూన్య సమితి_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_సమయం,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  పూర్ణాంకం arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (శూన్య)tmrs_settimer(&clock_timers, tp, exp_సమయం, watchdog, arg, ఖాళీ, ఖాళీ);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
శూన్య reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  ఒకవేళ (tmr_is_సమితి(tp))
	(శూన్య)tmrs_clrtimer(&clock_timers, tp, ఖాళీ, ఖాళీ);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
స్థిర శూన్య ఎక్కించు_update(శూన్య)
{
	u16_t స్థానం;
	పూర్ణాంకం enqueued = 0, q;
	నిర్మాణం proc *p;
	నిర్మాణం proc **rdy_తల;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	స్థానం = (kclockinfo.uptime / system_hz / ఎక్కించు_UNIT_SECS) %
	    ఎక్కించు_HISTORY;
	ఒకవేళ(స్థానం != kloadinfo.proc_చివరి_స్థానం) {
		kloadinfo.proc_ఎక్కించు_history[స్థానం] = 0;
		kloadinfo.proc_చివరి_స్థానం = స్థానం;
	}

	rdy_తల = get_cpulocal_var(నిరంతరం_q_తల);
	/* Cumulation. How many processes are ready now? */
	కొరకు(q = 0; q < NR_SCHED_QUEUES; q++) {
		కొరకు(p = rdy_తల[q]; p != ఖాళీ; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_ఎక్కించు_history[స్థానం] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.చివరి_clock = kclockinfo.uptime;
}

పూర్ణాంకం boot_cpu_init_timer(గుర్తులేని freq)
{
	ఒకవేళ (init_local_timer(freq))
		తిరిగి -1;

	ఒకవేళ (రిజిస్టరు_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		తిరిగి -1;

	తిరిగి 0;
}

పూర్ణాంకం app_cpu_init_timer(గుర్తులేని freq)
{
	ఒకవేళ (init_local_timer(freq))
		తిరిగి -1;

	తిరిగి 0;
}
