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
ціле do_iopenable(struct proc * caller, message * m_ptr)
{
  ціле proc_nr;

#якщо 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  якщо (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } інакше якщо(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	повернути(EINVAL);
  enable_iop(proc_addr(proc_nr));
  повернути(OK);
#інакше
  повернути(EPERM);
#endif
}


