/* The kernel call implemented in this file:
 *   m_type:	SYS_SETTIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_settime.now
 *   m_lsys_krn_sys_settime.clock_id
 *   m_lsys_krn_sys_settime.sec
 *   m_lsys_krn_sys_settime.nsec
 */

#समावेश "kernel/system.h"
#समावेश <minix/endpoint.h>
#समावेश <time.h>

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
पूर्णांक do_settime(काष्ठा proc * caller, message * m_ptr)
{
  घड़ी_प्रकार newclock;
  int32_t ticks;
  समय_प्रकार boottime, timediff, timediff_ticks;

  /* only realtime can change */
  अगर (m_ptr->m_lsys_krn_sys_settime.clock_id != CLOCK_REALTIME)
	वापस EINVAL;

  /* user just wants to adjtime() */
  अगर (m_ptr->m_lsys_krn_sys_settime.now == 0) {
	/* convert delta value from seconds and nseconds to ticks */
	ticks = (m_ptr->m_lsys_krn_sys_settime.sec * system_hz) +
		(m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));
	set_adjtime_delta(ticks);
	वापस(OK);
  } /* else user wants to set the time */

  boottime = get_boottime();

  timediff = m_ptr->m_lsys_krn_sys_settime.sec - boottime;
  timediff_ticks = timediff * system_hz;

  /* prevent a negative value for realtime */
  अगर (m_ptr->m_lsys_krn_sys_settime.sec <= boottime ||
      timediff_ticks < दीर्घ_न्यून/2 || timediff_ticks > दीर्घ_उच्च/2) {
  	/* boottime was likely wrong, try to correct it. */
	set_boottime(m_ptr->m_lsys_krn_sys_settime.sec);
	set_realtime(1);
	वापस(OK);
  }

  /* calculate the new value of realtime in ticks */
  newclock = timediff_ticks +
      (m_ptr->m_lsys_krn_sys_settime.nsec/(1000000000/system_hz));

  set_realtime(newclock);

  वापस(OK);
}
