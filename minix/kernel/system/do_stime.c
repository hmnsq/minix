/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#অন্তর্ভাব "kernel/system.h"

#অন্তর্ভাব <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_stime(কাঠামো proc * caller, message * m_ptr)
{
  set_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  ফেরত(OK);
}
