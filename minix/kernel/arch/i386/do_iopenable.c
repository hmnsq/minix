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

#ᤀᤡ᤺ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ᤀᤡ᤺ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ᤀᤡᤴ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ᤀᤡᤴ(OK);
#ᤀᤡ᤺ᤆᤡ
  ᤀᤡᤴ(EPERM);
#endif
}


