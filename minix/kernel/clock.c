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

#அந்தர்பாவ <minix/endpoint.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <assert.h>

#அந்தர்பாவ "clock.h"

#ifdef USE_WATCHDOG
#அந்தர்பாவ "watchdog.h"
#endif

/* Function prototype for PRIVATE functions.
 */
நிலையான வெறுமை load_update(வெறுமை);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
நிலையான minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
நிலையான int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
வெறுமை
init_clock(வெறுமை)
{
	எழுத்து *மதிப்பு;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, அளவு(kclockinfo));

	/* Get clock tick frequency. */
	மதிப்பு = env_get("hz");
	எனில் (மதிப்பு != வெற்று)
		kclockinfo.hz = atoi(மதிப்பு);
	எனில் (மதிப்பு == வெற்று || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, அளவு(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
முழு timer_int_handler(வெறுமை)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	கட்டமைப்பு proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#ifdef USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#endif

	எனில் (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		எனில் (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} இல்லையெனில் {
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

	எனில் (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	எனில் ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_இடது > 0)) {
		p->p_virt_இடது--;
	}
	எனில் ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_இடது > 0)) {
		p->p_prof_இடது--;
	}
	எனில் (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_இடது > 0)) {
		billp->p_prof_இடது--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	எனில் (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	எனில் (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		எனில் (clock_timers != வெற்று &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, வெற்று);

#ifdef DEBUG_SERIAL
		எனில் (kinfo.do_serial_debug)
			do_ser_debug();
#endif

	}

	arch_timer_int_handler();

	திரும்பு(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(வெறுமை)
{
  /* Get and return the current wall time in ticks since boot. */
  திரும்பு(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
வெறுமை set_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
வெறுமை set_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(வெறுமை)
{
  /* Get and return the number of ticks since boot. */
  திரும்பு(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
வெறுமை set_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(வெறுமை)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  திரும்பு(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
வெறுமை set_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_time,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  முழு arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (வெறுமை)tmrs_settimer(&clock_timers, tp, exp_time, watchdog, arg, வெற்று, வெற்று);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
வெறுமை reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  எனில் (tmr_is_set(tp))
	(வெறுமை)tmrs_clrtimer(&clock_timers, tp, வெற்று, வெற்று);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
நிலையான வெறுமை load_update(வெறுமை)
{
	u16_t slot;
	முழு enqueued = 0, q;
	கட்டமைப்பு proc *p;
	கட்டமைப்பு proc **rdy_head;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (kclockinfo.uptime / system_hz / _LOAD_UNIT_SECS) %
	    _LOAD_HISTORY;
	எனில்(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	rdy_head = get_cpulocal_var(run_q_head);
	/* Cumulation. How many processes are ready now? */
	க்கு(q = 0; q < NR_SCHED_QUEUES; q++) {
		க்கு(p = rdy_head[q]; p != வெற்று; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = kclockinfo.uptime;
}

முழு boot_cpu_init_timer(unsigned freq)
{
	எனில் (init_local_timer(freq))
		திரும்பு -1;

	எனில் (register_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		திரும்பு -1;

	திரும்பு 0;
}

முழு app_cpu_init_timer(unsigned freq)
{
	எனில் (init_local_timer(freq))
		திரும்பு -1;

	திரும்பு 0;
}
