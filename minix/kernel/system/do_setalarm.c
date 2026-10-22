/* The kernel call implemented in this file:
 *   m_type:	SYS_SETALARM 
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_setalarm.exp_time		(alarm's expiration time)
 *    m_lsys_krn_sys_setalarm.abs_time		(expiration time is absolute?)
 *    m_lsys_krn_sys_setalarm.time_left		(return seconds left of previous)
 */

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ <assert.h>

#ఒకవేళ USE_SETALARM

స్థిర శూన్య cause_alarm(పూర్ణాంకం proc_nr_e);

/*===========================================================================*
 *				do_setalarm				     *
 *===========================================================================*/
పూర్ణాంకం do_setalarm(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* A process requests a synchronous alarm, or wants to cancel its alarm. */
  దీర్ఘ exp_సమయం;		/* expiration time for this alarm */
  పూర్ణాంకం use_abs_సమయం;		/* use absolute or relative time */
  minix_timer_t *tp;		/* the process' timer structure */
  clock_t uptime;		/* placeholder for current uptime */

  /* Extract shared parameters from the request message. */
  exp_సమయం = m_ptr->m_lsys_krn_sys_setalarm.exp_సమయం;
  use_abs_సమయం = m_ptr->m_lsys_krn_sys_setalarm.abs_సమయం;
  ఒకవేళ (! (priv(caller)->s_flags & SYS_PROC)) తిరిగి(EPERM);

  /* Get the timer structure and set the parameters for this alarm. */
  tp = &(priv(caller)->s_alarm_timer);

  /* Return the ticks left on the previous alarm. */
  uptime = get_monotonic(); 
  ఒకవేళ (!tmr_is_సమితి(tp)) {
	m_ptr->m_lsys_krn_sys_setalarm.సమయం_ఎడమ = TMR_NEVER;
  } లేకపోతే ఒకవేళ (tmr_is_మొదటి(uptime, tp->tmr_exp_సమయం)) {
	m_ptr->m_lsys_krn_sys_setalarm.సమయం_ఎడమ = tp->tmr_exp_సమయం - uptime;
  } లేకపోతే {
	m_ptr->m_lsys_krn_sys_setalarm.సమయం_ఎడమ = 0;
  }

  /* For the caller's convenience, also return the current time. */
  m_ptr->m_lsys_krn_sys_setalarm.uptime = uptime;

  /*
   * Finally, (re)set the timer depending on the expiration time.  Note that
   * an absolute time of zero is as valid as any other absolute value, so only
   * a relative time value of zero resets the timer.
   */
  ఒకవేళ (!use_abs_సమయం && exp_సమయం == 0) {
	reset_kernel_timer(tp);
  } లేకపోతే {
	ఒకవేళ (!use_abs_సమయం)
		exp_సమయం += uptime;
	సమితి_kernel_timer(tp, exp_సమయం, cause_alarm, caller->p_endpoint);
  }
  తిరిగి(OK);
}

/*===========================================================================*
 *				cause_alarm				     *
 *===========================================================================*/
స్థిర శూన్య cause_alarm(పూర్ణాంకం proc_nr_e)
{
/* Routine called if a timer goes off and the process requested a synchronous
 * alarm. The process number is stored as the timer argument. Notify that
 * process with a notification message from CLOCK.
 */
  mini_notify(proc_addr(CLOCK), proc_nr_e);	/* notify process */
}

#అయితే_ముగింపు /* USE_SETALARM */
