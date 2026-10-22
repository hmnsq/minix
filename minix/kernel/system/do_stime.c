/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
ਪੂਰਨ do_stime(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਸੈੱਟ_boottime(m_ptr->m_lsys_krn_sys_stime.boot_ਸਮਾਂ);
  ਵਾਪਸ(OK);
}
