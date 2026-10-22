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
จำนวนเต็ม do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  จำนวนเต็ม proc_nr;

#ถ้า 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ถ้า (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } มิฉะนั้น ถ้า(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	คืน(EINVAL);
  enable_iop(proc_addr(proc_nr));
  คืน(OK);
#มิฉะนั้น
  คืน(EPERM);
#endif
}


