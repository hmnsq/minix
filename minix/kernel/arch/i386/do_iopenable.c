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
පූර්ණ_සංඛ්‍යා do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  පූර්ණ_සංඛ්‍යා proc_nr;

#නම් 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  නම් (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } නැතහොත් නම්(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ආපසු(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ආපසු(OK);
#නැතහොත්
  ආපසු(EPERM);
#endif
}


