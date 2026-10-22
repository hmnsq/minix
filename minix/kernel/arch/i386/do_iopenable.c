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

#ᯀᯮᯝ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ᯀᯮᯝ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ᯀᯮᯞ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ᯀᯮᯞ(OK);
#ᯀᯮᯝᯂᯪ
  ᯀᯮᯞ(EPERM);
#endif
}


