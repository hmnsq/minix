/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#شامل "kernel/system.h"
#شامل <minix/endpoint.h>
#شامل <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
صحیح do_settime(ساخت proc * caller, پیغام * m_ptr)
{
  clock_t newclock;
  int32_t ticks;
  time_t boottime, timediff, timediff_ticks;

  /* only realtime can change */
  اگر (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	واپس EINVAL;

  /* user just wants to adjtime() */
  اگر (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	سیٹ_adjtime_delta(ticks);
	واپس(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  اگر (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < LONG_MIN/2 || timediff_ticks > LONG_MAX/2) {
  	/* boottime was likely wrong, try to correct it. */
	سیٹ_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	سیٹ_realtime(1);
	واپس(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  سیٹ_realtime(newclock);

  واپس(OK);
}
