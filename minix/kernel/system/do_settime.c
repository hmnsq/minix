/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
ਪੂਰਨ do_settime(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  clock_t newclock;
  int32_t ticks;
  time_t boottime, timediff, timediff_ticks;

  /* only realtime can change */
  ਜੇ (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	ਵਾਪਸ EINVAL;

  /* user just wants to adjtime() */
  ਜੇ (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	ਸੈੱਟ_adjtime_delta(ticks);
	ਵਾਪਸ(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  ਜੇ (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < LONG_MIN/2 || timediff_ticks > LONG_MAX/2) {
  	/* boottime was likely wrong, try to correct it. */
	ਸੈੱਟ_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	ਸੈੱਟ_realtime(1);
	ਵਾਪਸ(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  ਸੈੱਟ_realtime(newclock);

  ਵਾਪਸ(OK);
}
