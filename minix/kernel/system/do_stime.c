/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#ضمن "kernel/system.h"

#ضمن <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
صحيح do_stime(بنية proc * caller, پیغام * m_ptr)
{
  سېټ_boottime(m_ptr->m_lsys_krn_sys_stime.boot_وخت);
  أرجع(OK);
}
