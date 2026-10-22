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

#અન્તર્ભાવ <minix/endpoint.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <assert.h>

#અન્તર્ભાવ "clock.h"

#ifdef USE_WATCHDOG
#અન્તર્ભાવ "watchdog.h"
#endif

/* Function prototype for PRIVATE functions.
 */
સ્થિર શૂન્ય load_update(શૂન્ય);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
સ્થિર minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
સ્થિર int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
શૂન્ય
init_clock(શૂન્ય)
{
	અક્ષર *કિંમત;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, કદ(kclockinfo));

	/* Get clock tick frequency. */
	કિંમત = env_get("hz");
	જો (કિંમત != ખાલી)
		kclockinfo.hz = atoi(કિંમત);
	જો (કિંમત == ખાલી || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, કદ(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
પૂર્ણાંક timer_int_handler(શૂન્ય)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	સંરચના proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#ifdef USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#endif

	જો (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		જો (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} નહીંતર {
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

	જો (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	જો ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_ડાબું > 0)) {
		p->p_virt_ડાબું--;
	}
	જો ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_ડાબું > 0)) {
		p->p_prof_ડાબું--;
	}
	જો (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_ડાબું > 0)) {
		billp->p_prof_ડાબું--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	જો (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	જો (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		જો (clock_timers != ખાલી &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, ખાલી);

#ifdef DEBUG_SERIAL
		જો (kinfo.do_serial_debug)
			do_ser_debug();
#endif

	}

	arch_timer_int_handler();

	પાછા(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(શૂન્ય)
{
  /* Get and return the current wall time in ticks since boot. */
  પાછા(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
શૂન્ય set_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
શૂન્ય set_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(શૂન્ય)
{
  /* Get and return the number of ticks since boot. */
  પાછા(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
શૂન્ય set_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(શૂન્ય)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  પાછા(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
શૂન્ય set_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_time,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  પૂર્ણાંક arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (શૂન્ય)tmrs_settimer(&clock_timers, tp, exp_time, watchdog, arg, ખાલી, ખાલી);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
શૂન્ય reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  જો (tmr_is_set(tp))
	(શૂન્ય)tmrs_clrtimer(&clock_timers, tp, ખાલી, ખાલી);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
સ્થિર શૂન્ય load_update(શૂન્ય)
{
	u16_t slot;
	પૂર્ણાંક enqueued = 0, q;
	સંરચના proc *p;
	સંરચના proc **rdy_head;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (kclockinfo.uptime / system_hz / _LOAD_UNIT_SECS) %
	    _LOAD_HISTORY;
	જો(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	rdy_head = get_cpulocal_var(run_q_head);
	/* Cumulation. How many processes are ready now? */
	માટે(q = 0; q < NR_SCHED_QUEUES; q++) {
		માટે(p = rdy_head[q]; p != ખાલી; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = kclockinfo.uptime;
}

પૂર્ણાંક boot_cpu_init_timer(unsigned freq)
{
	જો (init_local_timer(freq))
		પાછા -1;

	જો (register_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		પાછા -1;

	પાછા 0;
}

પૂર્ણાંક app_cpu_init_timer(unsigned freq)
{
	જો (init_local_timer(freq))
		પાછા -1;

	પાછા 0;
}
