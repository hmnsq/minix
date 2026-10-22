/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#অন্তর্ভাৱ "kernel/system.h"

#অন্তর্ভাৱ <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_stime(সংরচনা proc * caller, message * m_ptr)
{
  set_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  ঘূৰাই(OK);
}
