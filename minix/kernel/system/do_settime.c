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
पूर्णांक do_settime(संरचना proc * caller, message * m_ptr)
{
  घड़ी_प्रकार newclock;
  int32_t ticks;
  समय_प्रकार boottime, timediff, timediff_ticks;

  /* only realtime can change */
  जर (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	परत EINVAL;

  /* user just wants to adjtime() */
  जर (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	set_adjtime_delta(ticks);
	परत(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  जर (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < दीर्घ_न्यून/2 || timediff_ticks > दीर्घ_उच्च/2) {
  	/* boottime was likely wrong, try to correct it. */
	set_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	set_realtime(1);
	परत(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  set_realtime(newclock);

  परत(OK);
}
