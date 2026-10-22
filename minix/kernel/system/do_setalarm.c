/* The kernel call implemented in this file:
 *   m_type:	SYS_SETALARM 
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_setalarm.exp_time		(alarm's expiration time)
 *    m_lsys_krn_sys_setalarm.abs_time		(expiration time is absolute?)
 *    m_lsys_krn_sys_setalarm.time_left		(return seconds left of previous)
 */

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव <assert.h>

#यदि USE_SETALARM

स्थिर शून्य cause_alarm(पूर्णाङ्क proc_nr_e);

/*===========================================================================*
 *				do_setalarm				     *
 *===========================================================================*/
पूर्णाङ्क do_setalarm(संरचना proc * caller, सन्देशः * m_ptr)
{
/* A process requests a synchronous alarm, or wants to cancel its alarm. */
  दीर्घ exp_time;		/* expiration time for this alarm */
  पूर्णाङ्क use_abs_time;		/* use absolute or relative time */
  minix_timer_t *tp;		/* the process' timer structure */
  clock_t uptime;		/* placeholder for current uptime */

  /* Extract shared parameters from the request message. */
  exp_time = m_ptr->m_lsys_krn_sys_setalarm.exp_time;
  use_abs_time = m_ptr->m_lsys_krn_sys_setalarm.abs_time;
  यदि (! (priv(caller)->s_flags & SYS_PROC)) प्रत्यागम(EPERM);

  /* Get the timer structure and set the parameters for this alarm. */
  tp = &(priv(caller)->s_alarm_timer);

  /* Return the ticks left on the previous alarm. */
  uptime = get_monotonic(); 
  यदि (!tmr_is_समुच्चयः(tp)) {
	m_ptr->m_lsys_krn_sys_setalarm.time_वामः = TMR_NEVER;
  } अन्यथा यदि (tmr_is_first(uptime, tp->tmr_exp_time)) {
	m_ptr->m_lsys_krn_sys_setalarm.time_वामः = tp->tmr_exp_time - uptime;
  } अन्यथा {
	m_ptr->m_lsys_krn_sys_setalarm.time_वामः = 0;
  }

  /* For the caller's convenience, also return the current time. */
  m_ptr->m_lsys_krn_sys_setalarm.uptime = uptime;

  /*
   * Finally, (re)set the timer depending on the expiration time.  Note that
   * an absolute time of zero is as valid as any other absolute value, so only
   * a relative time value of zero resets the timer.
   */
  यदि (!use_abs_time && exp_time == 0) {
	reset_kernel_timer(tp);
  } अन्यथा {
	यदि (!use_abs_time)
		exp_time += uptime;
	समुच्चयः_kernel_timer(tp, exp_time, cause_alarm, caller->p_endpoint);
  }
  प्रत्यागम(OK);
}

/*===========================================================================*
 *				cause_alarm				     *
 *===========================================================================*/
स्थिर शून्य cause_alarm(पूर्णाङ्क proc_nr_e)
{
/* Routine called if a timer goes off and the process requested a synchronous
 * alarm. The process number is stored as the timer argument. Notify that
 * process with a notification message from CLOCK.
 */
  mini_notify(proc_addr(CLOCK), proc_nr_e);	/* notify process */
}

#यदि_अन्तः /* USE_SETALARM */
