/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#شامل "kernel/system.h"

#شامل <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
صحیح do_stime(ساخت proc * caller, پیغام * m_ptr)
{
  سیٹ_boottime(m_ptr->m_lsys_krn_sys_stime.boot_وقت);
  واپس(OK);
}
