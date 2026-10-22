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
ចំនួនគត់ do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  ចំនួនគត់ proc_nr;

#ប្រសិនបើ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ប្រសិនបើ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } បើមិនដូច្នេះ ប្រសិនបើ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ត្រឡប់(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ត្រឡប់(OK);
#បើមិនដូច្នេះ
  ត្រឡប់(EPERM);
#endif
}


