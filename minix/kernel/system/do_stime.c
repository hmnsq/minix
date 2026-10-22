/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#समावेश "kernel/system.h"

#समावेश <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
पूर्णांक do_stime(काष्ठा proc * caller, message * m_ptr)
{
  set_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  वापस(OK);
}
