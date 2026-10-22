/* The kernel call implemented in this file:
 *   m_type:	SYS_SETALARM 
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_setalarm.exp_time		(alarm's expiration time)
 *    m_lsys_krn_sys_setalarm.abs_time		(expiration time is absolute?)
 *    m_lsys_krn_sys_setalarm.time_left		(return seconds left of previous)
 */

#ضمن "kernel/system.h"

#ضمن <minix/endpoint.h>
#ضمن <assert.h>

#إذا USE_SETALARM

ساكن فراغ cause_alarm(صحيح proc_nr_e);

/*===========================================================================*
 *				do_setalarm				     *
 *===========================================================================*/
صحيح do_setalarm(بنية proc * caller, message * m_ptr)
{
/* A process requests a synchronous alarm, or wants to cancel its alarm. */
  long exp_time;		/* expiration time for this alarm */
  صحيح use_abs_time;		/* use absolute or relative time */
  minix_timer_t *tp;		/* the process' timer structure */
  clock_t uptime;		/* placeholder for current uptime */

  /* Extract shared parameters from the request message. */
  exp_time = m_ptr->m_lsys_krn_sys_setalarm.exp_time;
  use_abs_time = m_ptr->m_lsys_krn_sys_setalarm.abs_time;
  إذا (! (priv(caller)->s_flags & SYS_PROC)) أرجع(EPERM);

  /* Get the timer structure and set the parameters for this alarm. */
  tp = &(priv(caller)->s_alarm_timer);

  /* Return the ticks left on the previous alarm. */
  uptime = get_monotonic(); 
  إذا (!tmr_is_set(tp)) {
	m_ptr->m_lsys_krn_sys_setalarm.time_هغو = TMR_NEVER;
  } وإلا إذا (tmr_is_first(uptime, tp->tmr_exp_time)) {
	m_ptr->m_lsys_krn_sys_setalarm.time_هغو = tp->tmr_exp_time - uptime;
  } وإلا {
	m_ptr->m_lsys_krn_sys_setalarm.time_هغو = 0;
  }

  /* For the caller's convenience, also return the current time. */
  m_ptr->m_lsys_krn_sys_setalarm.uptime = uptime;

  /*
   * Finally, (re)set the timer depending on the expiration time.  Note that
   * an absolute time of zero is as valid as any other absolute value, so only
   * a relative time value of zero resets the timer.
   */
  إذا (!use_abs_time && exp_time == 0) {
	reset_kernel_timer(tp);
  } وإلا {
	إذا (!use_abs_time)
		exp_time += uptime;
	set_kernel_timer(tp, exp_time, cause_alarm, caller->p_endpoint);
  }
  أرجع(OK);
}

/*===========================================================================*
 *				cause_alarm				     *
 *===========================================================================*/
ساكن فراغ cause_alarm(صحيح proc_nr_e)
{
/* Routine called if a timer goes off and the process requested a synchronous
 * alarm. The process number is stored as the timer argument. Notify that
 * process with a notification message from CLOCK.
 */
  mini_notify(proc_addr(CLOCK), proc_nr_e);	/* notify process */
}

#endif /* USE_SETALARM */
