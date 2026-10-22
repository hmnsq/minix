/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
పూర్ణాంకం do_stime(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  సమితి_boottime(m_ptr->m_lsys_krn_sys_stime.boot_సమయం);
  తిరిగి(OK);
}
