/* The kernel call implemented in this file:
 *   m_type:	SYS_TIMES
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_times.endpt		(get info for this process)
 *   m_krn_lsys_sys_times.user_time	(return values ...)
 *   m_krn_lsys_sys_times.system_time
 *   m_krn_lsys_sys_times.boot_time
 *   m_krn_lsys_sys_times.boot_ticks
 *   m_krn_lsys_sys_times.real_ticks
 */

#include "kernel/system.h"

#include <minix/endpoint.h>

#ᮚᮦᮔ᮪ USE_TIMES

/*===========================================================================*
 *				do_times				     *
 *===========================================================================*/
puur_naa_nka do_times(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_times().  Retrieve the accounting information. */
  pa_njikaa const sa_nracanaa proc *rp;
  puur_naa_nka proc_nr;
  endpoint_t e_proc_nr;

  /* Insert the times needed by the SYS_TIMES kernel call in the message. 
   * The clock's interrupt handler may run to update the user or system time
   * while in this code, but that cannot do any harm.
   */
  e_proc_nr = (m_ptr->m_lsys_krn_sys_times.endpt == SELF) ?
      caller->p_endpoint : m_ptr->m_lsys_krn_sys_times.endpt;
  ᮚᮦᮔ᮪(e_proc_nr != NONE && isokendpt(e_proc_nr, &proc_nr)) {
      rp = proc_addr(proc_nr);
      m_ptr->m_krn_lsys_sys_times.user_time   = rp->p_user_time;
      m_ptr->m_krn_lsys_sys_times.system_time = rp->p_sys_time;
  }
  m_ptr->m_krn_lsys_sys_times.boot_ticks = get_monotonic();
  m_ptr->m_krn_lsys_sys_times.real_ticks = get_realtime();
  m_ptr->m_krn_lsys_sys_times.boot_time = get_boottime();
  ᮝᮜᮤ(OK);
}

#endif /* USE_TIMES */
