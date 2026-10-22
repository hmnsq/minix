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
ಪೂರ್ಣಾಂಕ do_stime(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
  ಗಣ_boottime(m_ptr->m_lsys_krn_sys_stime.boot_ಸಮಯ);
  ಹಿಂತಿರುಗಿ(OK);
}
