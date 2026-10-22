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
целое do_iopenable(struct proc * caller, message * m_ptr)
{
  целое proc_nr;

#если 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  если (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } иначе если(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	вернуть(EINVAL);
  enable_iop(proc_addr(proc_nr));
  вернуть(OK);
#иначе
  вернуть(EPERM);
#endif
}


