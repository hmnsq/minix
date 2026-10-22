/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>
#অন্তর্ভাব <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_settime(কাঠামো proc * caller, message * m_ptr)
{
  clock_t newclock;
  int32_t ticks;
  time_t boottime, timediff, timediff_ticks;

  /* only realtime can change */
  যদি (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	ফেরত EINVAL;

  /* user just wants to adjtime() */
  যদি (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	set_adjtime_delta(ticks);
	ফেরত(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  যদি (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < LONG_MIN/2 || timediff_ticks > LONG_MAX/2) {
  	/* boottime was likely wrong, try to correct it. */
	set_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	set_realtime(1);
	ফেরত(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  set_realtime(newclock);

  ফেরত(OK);
}
