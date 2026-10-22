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
整数 do_iopenable(struct proc * caller, message * m_ptr)
{
  整数 proc_nr;

#もし 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  もし (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } 否则 もし(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	戻る(EINVAL);
  enable_iop(proc_addr(proc_nr));
  戻る(OK);
#否则
  戻る(EPERM);
#endif
}


