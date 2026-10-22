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
цео do_iopenable(struct proc * caller, message * m_ptr)
{
  цео proc_nr;

#ако 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ако (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } инако ако(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	врати(EINVAL);
  enable_iop(proc_addr(proc_nr));
  врати(OK);
#инако
  врати(EPERM);
#endif
}


