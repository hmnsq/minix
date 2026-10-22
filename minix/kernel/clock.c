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

#समावेश <minix/endpoint.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <assert.h>

#समावेश "clock.h"

#अगर_घोषित USE_WATCHDOG
#समावेश "watchdog.h"
#पूर्ण_अगर

/* Function prototype for PRIVATE functions.
 */
अटल व्योम load_update(व्योम);

/* The CLOCK's timers queue. The functions in <minix/timers.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_kernel_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
अटल minix_timer_t *clock_timers;	/* queue of CLOCK timers */

/* Number of ticks to adjust realtime by. A negative value implies slowing
 * down realtime, a positive value implies speeding it up.
 */
अटल int32_t adjtime_delta = 0;

/*
 * Initialize the clock variables.
 */
व्योम
init_clock(व्योम)
{
	अक्षर *मूल्य;

	/* Initialize clock information structure. */
	स_रखो(&kclockinfo, 0, माप(kclockinfo));

	/* Get clock tick frequency. */
	मूल्य = env_get("hz");
	अगर (मूल्य != शून्य)
		kclockinfo.hz = म_से_प(मूल्य);
	अगर (मूल्य == शून्य || kclockinfo.hz < 2 || kclockinfo.hz > 50000)
		kclockinfo.hz = DEFAULT_HZ;

	/* Load average data initialization. */
	स_रखो(&kloadinfo, 0, माप(kloadinfo));
}

/*
 * The boot processor's timer interrupt handler. In addition to non-boot cpus
 * it keeps real time and notifies the clock task if need be.
 */
पूर्णांक timer_int_handler(व्योम)
{
	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	काष्ठा proc * p, * billp;

	/* FIXME watchdog for slave cpus! */
#अगर_घोषित USE_WATCHDOG
	/*
	 * we need to know whether local timer ticks are happening or whether
	 * the kernel is locked up. We don't care about overflows as we only
	 * need to know that it's still ticking or not
	 */
	watchdog_local_timer_ticks++;
#पूर्ण_अगर

	अगर (cpu_is_bsp(cpuid)) {
		kclockinfo.uptime++;

		/* if adjtime_delta has ticks remaining, apply one to realtime.
		 * limit changes to every other interrupt.
		 */
		अगर (adjtime_delta != 0 && kclockinfo.uptime & 0x1) {
			/* go forward or stay behind */
			kclockinfo.realtime += (adjtime_delta > 0) ? 2 : 0;
			adjtime_delta += (adjtime_delta > 0) ? -1 : +1;
		} अन्यथा {
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

	अगर (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time++;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	अगर ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_बायाँ > 0)) {
		p->p_virt_बायाँ--;
	}
	अगर ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_बायाँ > 0)) {
		p->p_prof_बायाँ--;
	}
	अगर (! (priv(p)->s_flags & BILLABLE) &&
			(billp->p_misc_flags & MF_PROF_TIMER) &&
			(billp->p_prof_बायाँ > 0)) {
		billp->p_prof_बायाँ--;
	}

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	अगर (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	अगर (cpu_is_bsp(cpuid)) {
		/*
		 * If a timer expired, notify the clock task.  Keep in mind
		 * that clock tick values may overflow, so we must only look at
		 * relative differences, and only if there are timers at all.
		 */
		अगर (clock_timers != शून्य &&
		    tmr_has_expired(clock_timers, kclockinfo.uptime))
			tmrs_exptimers(&clock_timers, kclockinfo.uptime, शून्य);

#अगर_घोषित DEBUG_SERIAL
		अगर (kinfo.do_serial_debug)
			do_ser_debug();
#पूर्ण_अगर

	}

	arch_timer_int_handler();

	वापस(1);					/* reenable interrupts */
}

/*===========================================================================*
 *				get_realtime				     *
 *===========================================================================*/
घड़ी_प्रकार get_realtime(व्योम)
{
  /* Get and return the current wall time in ticks since boot. */
  वापस(kclockinfo.realtime);
}

/*===========================================================================*
 *				set_realtime				     *
 *===========================================================================*/
व्योम set_realtime(घड़ी_प्रकार newrealtime)
{
  kclockinfo.realtime = newrealtime;
}

/*===========================================================================*
 *				set_adjtime_delta			     *
 *===========================================================================*/
व्योम set_adjtime_delta(int32_t ticks)
{
  adjtime_delta = ticks;
}

/*===========================================================================*
 *				get_monotonic				     *
 *===========================================================================*/
घड़ी_प्रकार get_monotonic(व्योम)
{
  /* Get and return the number of ticks since boot. */
  वापस(kclockinfo.uptime);
}

/*===========================================================================*
 *				set_boottime				     *
 *===========================================================================*/
व्योम set_boottime(समय_प्रकार newboottime)
{
  kclockinfo.boottime = newboottime;
}

/*===========================================================================*
 *				get_boottime				     *
 *===========================================================================*/
समय_प्रकार get_boottime(व्योम)
{
  /* Get and return the number of seconds since the UNIX epoch. */
  वापस(kclockinfo.boottime);
}

/*===========================================================================*
 *				set_kernel_timer			     *
 *===========================================================================*/
व्योम set_kernel_timer(
  minix_timer_t *tp,			/* pointer to timer structure */
  घड़ी_प्रकार exp_time,			/* expiration monotonic time */
  tmr_func_t watchdog,			/* watchdog to be called */
  पूर्णांक arg				/* argument for watchdog function */
)
{
/* Insert the new timer in the active timers list. Always update the
 * next timeout time by setting it to the front of the active list.
 */
  (व्योम)tmrs_settimer(&clock_timers, tp, exp_time, watchdog, arg, शून्य, शून्य);
}

/*===========================================================================*
 *				reset_kernel_timer			     *
 *===========================================================================*/
व्योम reset_kernel_timer(
  minix_timer_t *tp			/* pointer to timer structure */
)
{
/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
 * active and expired lists. Always update the next timeout time by setting
 * it to the front of the active list.
 */
  अगर (tmr_is_set(tp))
	(व्योम)tmrs_clrtimer(&clock_timers, tp, शून्य, शून्य);
}

/*===========================================================================*
 *				load_update				     *
 *===========================================================================*/
अटल व्योम load_update(व्योम)
{
	u16_t slot;
	पूर्णांक enqueued = 0, q;
	काष्ठा proc *p;
	काष्ठा proc **rdy_शीर्ष;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (kclockinfo.uptime / system_hz / _LOAD_UNIT_SECS) %
	    _LOAD_HISTORY;
	अगर(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	rdy_शीर्ष = get_cpulocal_var(run_q_शीर्ष);
	/* Cumulation. How many processes are ready now? */
	क्रम(q = 0; q < NR_SCHED_QUEUES; q++) {
		क्रम(p = rdy_शीर्ष[q]; p != शून्य; p = p->p_nextready) {
			enqueued++;
		}
	}

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = kclockinfo.uptime;
}

पूर्णांक boot_cpu_init_timer(अचिन्हित freq)
{
	अगर (init_local_timer(freq))
		वापस -1;

	अगर (register_local_timer_handler(
				(irq_handler_t) timer_int_handler))
		वापस -1;

	वापस 0;
}

पूर्णांक app_cpu_init_timer(अचिन्हित freq)
{
	अगर (init_local_timer(freq))
		वापस -1;

	वापस 0;
}
