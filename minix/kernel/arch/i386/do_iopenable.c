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
ཧྲིལ do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  ཧྲིལ proc_nr;

#གལ་སྲིད 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  གལ་སྲིད (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } དེ་མིན གལ་སྲིད(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ལོག(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ལོག(OK);
#དེ་མིན
  ལོག(EPERM);
#endif
}


