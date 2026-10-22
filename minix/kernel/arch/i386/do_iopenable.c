/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#include "kernel/system.h"
#include <minix/endpoint.h>

#include "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
puur_naa_nka do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  puur_naa_nka proc_nr;

#ᮚᮦᮔ᮪ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ᮚᮦᮔ᮪ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ᮜᮤᮚ ᮚᮦᮔ᮪(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ᮝᮜᮤ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ᮝᮜᮤ(OK);
#ᮜᮤᮚ
  ᮝᮜᮤ(EPERM);
#endif
}


