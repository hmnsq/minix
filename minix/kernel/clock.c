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

#شامل <minix/endpoint.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <assert.h>

#شامل "clock.h"

#اگر_تعریف_شدہ USE_WATCHDOG
#شامل "watchdog.h"
#اگر_ختم

/* Function prototype for PRIVATE functions.
 */
ساکن خالی بھرو_update(خالی);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
ساکن minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
ساکن int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
خالی
init_clock(خالی)
{
	حرف *قدر;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, حجم(kclockinfo));

	/* Get clock tick frequency. */
	قدر = env_get("hz");
	اگر (قدر != تہی)
		kclockinfo.hz = atoi(قدر);
	اگر (قدر == تہی || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, حجم(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
صحیح timer_int_handler(خالی)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	ساخت proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#اگر_تعریف_شدہ USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#اگر_ختم

	اگر (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		اگر (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} ورنہ {
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

	p->p_user_وقت++;

	اگر (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_وقت++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	اگر ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_بایاں > 0)) {
		p->p_virt_بایاں--;
	}
	اگر ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_بایاں > 0)) {
		p->p_prof_بایاں--;
	}
	اگر (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_بایاں > 0)) {
		billp->p_prof_بایاں--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	اگر (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	بھرو_update();

	اگر (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		اگر (clock_timers != تہی &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, تہی);

#اگر_تعریف_شدہ DEBUG_SERIAL
		اگر (kinfo.do_serial_debug)
			do_ser_debug();
#اگر_ختم

	}

	arch_timer_int_handler();

	واپس(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(خالی)
{
  /* Get and return the current wall time in ticks since boot. */
  واپس(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
خالی سیٹ_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
خالی سیٹ_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(خالی)
{
  /* Get and return the number of ticks since boot. */
  واپس(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
خالی سیٹ_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(خالی)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  واپس(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
خالی سیٹ_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_وقت,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  صحیح arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (خالی)tmrs_settimer(&clock_timers, tp, exp_وقت, watchdog, arg, تہی, تہی);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
خالی reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  اگر (tmr_is_سیٹ(tp))
	(خالی)tmrs_clrtimer(&clock_timers, tp, تہی, تہی);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
ساکن خالی بھرو_update(خالی)
{
	u16_t خانہ;
	صحیح enqueued = 0, q;
	ساخت proc *p;
	ساخت proc **rdy_سر;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	خانہ = (kclockinfo.uptime / system_hz / بھرو_UNIT_SECS) %
	    بھرو_HISTORY;
	اگر(خانہ != kloadinfo.proc_آخری_خانہ) {
		kloadinfo.proc_بھرو_history[خانہ] = 0;
		kloadinfo.proc_آخری_خانہ = خانہ;
	}

	rdy_سر = get_cpulocal_var(تسلسل_q_سر);
	/* Cumulation. How many processes are ready now? */
	برائے(q = 0; q < NR_SCHED_QUEUES; q++) {
		برائے(p = rdy_سر[q]; p != تہی; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_بھرو_history[خانہ] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.آخری_clock = kclockinfo.uptime;
}

صحیح boot_cpu_init_timer(غیرعلامتی freq)
{
	اگر (init_local_timer(freq))
		واپس -1;

	اگر (رجسٹر_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		واپس -1;

	واپس 0;
}

صحیح app_cpu_init_timer(غیرعلامتی freq)
{
	اگر (init_local_timer(freq))
		واپس -1;

	واپس 0;
}
