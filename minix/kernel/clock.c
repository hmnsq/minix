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

#ضمن <minix/endpoint.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <assert.h>

#ضمن "clock.h"

#اگر_تعریف_شده USE_WATCHDOG
#ضمن "watchdog.h"
#پایان_اگر

/* Function prototype for PRIVATE functions.
 */
ساكن فراغ بارگیری_update(فراغ);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
ساكن minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
ساكن int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
فراغ
init_clock(فراغ)
{
	حرف *مقدار;

	/* Initialize clock information structure. */
	memset(&kclockinfo, 0, حجم(kclockinfo));

	/* Get clock tick frequency. */
	مقدار = env_get("hz");
	إذا (مقدار != عدم)
		kclockinfo.hz = atoi(مقدار);
	إذا (مقدار == عدم || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	memset(&kloadinfo, 0, حجم(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
صحيح timer_int_handler(فراغ)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	بنية proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#اگر_تعریف_شده USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#پایان_اگر

	إذا (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		إذا (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} وإلا {
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

	p->p_user_زمان++;

	إذا (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_زمان++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	إذا ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_چپ > 0)) {
		p->p_virt_چپ--;
	}
	إذا ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_چپ > 0)) {
		p->p_prof_چپ--;
	}
	إذا (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_چپ > 0)) {
		billp->p_prof_چپ--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	إذا (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	بارگیری_update();

	إذا (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		إذا (clock_timers != عدم &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, عدم);

#اگر_تعریف_شده DEBUG_SERIAL
		إذا (kinfo.do_serial_debug)
			do_ser_debug();
#پایان_اگر

	}

	arch_timer_int_handler();

	أرجع(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
clock_t get_realtime(فراغ)
{
  /* Get and return the current wall time in ticks since boot. */
  أرجع(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
فراغ مجموعه_realtime(clock_t newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
فراغ مجموعه_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
clock_t get_monotonic(فراغ)
{
  /* Get and return the number of ticks since boot. */
  أرجع(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
فراغ مجموعه_boottime(time_t newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
time_t get_boottime(فراغ)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  أرجع(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
فراغ مجموعه_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  clock_t exp_زمان,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  صحيح arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (فراغ)tmrs_settimer(&clock_timers, tp, exp_زمان, watchdog, arg, عدم, عدم);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
فراغ reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  إذا (tmr_is_مجموعه(tp))
	(فراغ)tmrs_clrtimer(&clock_timers, tp, عدم, عدم);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
ساكن فراغ بارگیری_update(فراغ)
{
	u16_t جایگاه;
	صحيح enqueued = 0, q;
	بنية proc *p;
	بنية proc **rdy_سر;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	جایگاه = (kclockinfo.uptime / system_hz / بارگیری_UNIT_SECS) %
	    بارگیری_HISTORY;
	إذا(جایگاه != kloadinfo.proc_آخرین_جایگاه) {
		kloadinfo.proc_بارگیری_history[جایگاه] = 0;
		kloadinfo.proc_آخرین_جایگاه = جایگاه;
	}

	rdy_سر = get_cpulocal_var(پیاپی_q_سر);
	/* Cumulation. How many processes are ready now? */
	لكل(q = 0; q < NR_SCHED_QUEUES; q++) {
		لكل(p = rdy_سر[q]; p != عدم; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_بارگیری_history[جایگاه] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.آخرین_clock = kclockinfo.uptime;
}

صحيح boot_cpu_init_timer(بیعلامت freq)
{
	إذا (init_local_timer(freq))
		أرجع -1;

	إذا (ثبات_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		أرجع -1;

	أرجع 0;
}

صحيح app_cpu_init_timer(بیعلامت freq)
{
	إذا (init_local_timer(freq))
		أرجع -1;

	أرجع 0;
}
