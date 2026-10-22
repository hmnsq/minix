/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
पूर्णाङ्क do_settime(संरचना proc * caller, सन्देशः * m_ptr)
{
  clock_t newclock;
  int32_t ticks;
  time_t boottime, timediff, timediff_ticks;

  /* only realtime can change */
  यदि (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	प्रत्यागम EINVAL;

  /* user just wants to adjtime() */
  यदि (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	समुच्चयः_adjtime_delta(ticks);
	प्रत्यागम(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  यदि (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < LONG_MIN/2 || timediff_ticks > LONG_MAX/2) {
  	/* boottime was likely wrong, try to correct it. */
	समुच्चयः_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	समुच्चयः_realtime(1);
	प्रत्यागम(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  समुच्चयः_realtime(newclock);

  प्रत्यागम(OK);
}
