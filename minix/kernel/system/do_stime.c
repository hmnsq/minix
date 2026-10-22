/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
முழு do_stime(கட்டமைப்பு proc * caller, message * m_ptr)
{
  set_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  திரும்பு(OK);
}
