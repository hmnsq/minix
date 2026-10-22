/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_stime(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  set_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  ಪ್ರತ್ಯಾಗಮ(OK);
}
