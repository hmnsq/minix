/* The kernel call implemented in this file:
 *   m_type:	SYS_SETALARM 
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_setalarm.exp_time		(alarm's expiration time)
 *    m_lsys_krn_sys_setalarm.abs_time		(expiration time is absolute?)
 *    m_lsys_krn_sys_setalarm.time_left		(return seconds left of previous)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ USE_SETALARM

ਸਥਿਰ ਖਾਲੀ cause_alarm(ਪੂਰਨ proc_nr_e);

/*===========================================================================*
 *				do_setalarm				     *
 *===========================================================================*/
ਪੂਰਨ do_setalarm(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* A process requests a synchronous alarm, or wants to cancel its alarm. */
  ਲੰਮਾ exp_ਸਮਾਂ;		/* expiration time for this alarm */
  ਪੂਰਨ use_abs_ਸਮਾਂ;		/* use absolute or relative time */
  minix_timer_t *tp;		/* the process' timer structure */
  clock_t uptime;		/* placeholder for current uptime */

  /* Extract shared parameters from the request message. */
  exp_ਸਮਾਂ = m_ptr->m_lsys_krn_sys_setalarm.exp_ਸਮਾਂ;
  use_abs_ਸਮਾਂ = m_ptr->m_lsys_krn_sys_setalarm.abs_ਸਮਾਂ;
  ਜੇ (! (priv(caller)->s_flags & SYS_PROC)) ਵਾਪਸ(EPERM);

  /* Get the timer structure and set the parameters for this alarm. */
  tp = &(priv(caller)->s_alarm_timer);

  /* Return the ticks left on the previous alarm. */
  uptime = get_monotonic(); 
  ਜੇ (!tmr_is_ਸੈੱਟ(tp)) {
	m_ptr->m_lsys_krn_sys_setalarm.ਸਮਾਂ_ਖੱਬਾ = TMR_NEVER;
  } ਨਹੀਂਤਾਂ ਜੇ (tmr_is_ਪਹਿਲਾ(uptime, tp->tmr_exp_ਸਮਾਂ)) {
	m_ptr->m_lsys_krn_sys_setalarm.ਸਮਾਂ_ਖੱਬਾ = tp->tmr_exp_ਸਮਾਂ - uptime;
  } ਨਹੀਂਤਾਂ {
	m_ptr->m_lsys_krn_sys_setalarm.ਸਮਾਂ_ਖੱਬਾ = 0;
  }

  /* For the caller's convenience, also return the current time. */
  m_ptr->m_lsys_krn_sys_setalarm.uptime = uptime;

  /*
   * Finally, (re)set the timer depending on the expiration time.  Note that
   * an absolute time of zero is as valid as any other absolute value, so only
   * a relative time value of zero resets the timer.
   */
  ਜੇ (!use_abs_ਸਮਾਂ && exp_ਸਮਾਂ == 0) {
	reset_kernel_timer(tp);
  } ਨਹੀਂਤਾਂ {
	ਜੇ (!use_abs_ਸਮਾਂ)
		exp_ਸਮਾਂ += uptime;
	ਸੈੱਟ_kernel_timer(tp, exp_ਸਮਾਂ, cause_alarm, caller->p_endpoint);
  }
  ਵਾਪਸ(OK);
}

/*===========================================================================*
 *				cause_alarm				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ cause_alarm(ਪੂਰਨ proc_nr_e)
{
/* Routine called if a timer goes off and the process requested a synchronous
 * alarm. The process number is stored as the timer argument. Notify that
 * process with a notification message from CLOCK.
 */
  mini_notify(proc_addr(CLOCK), proc_nr_e);	/* notify process */
}

#ਜੇ_ਅੰਤ /* USE_SETALARM */
