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
정수 do_iopenable(struct proc * caller, message * m_ptr)
{
  정수 proc_nr;

#만약 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  만약 (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } 아니면 만약(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	반환(EINVAL);
  enable_iop(proc_addr(proc_nr));
  반환(OK);
#아니면
  반환(EPERM);
#endif
}


