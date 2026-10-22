/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
పూర్ణాంకం do_settime(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  clock_t newclock;
  int32_t ticks;
  time_t boottime, timediff, timediff_ticks;

  /* only realtime can change */
  ఒకవేళ (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	తిరిగి EINVAL;

  /* user just wants to adjtime() */
  ఒకవేళ (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	సమితి_adjtime_delta(ticks);
	తిరిగి(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  ఒకవేళ (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < LONG_MIN/2 || timediff_ticks > LONG_MAX/2) {
  	/* boottime was likely wrong, try to correct it. */
	సమితి_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	సమితి_realtime(1);
	తిరిగి(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  సమితి_realtime(newclock);

  తిరిగి(OK);
}
