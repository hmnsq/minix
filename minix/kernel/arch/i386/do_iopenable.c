/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_iopenable(ସଂରଚନା proc * caller, message * m_ptr)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;

#ଯଦି 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ଯଦି (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ଅନ୍ୟଥା ଯଦି(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ଫେରନ୍ତୁ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ଫେରନ୍ତୁ(OK);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ(EPERM);
#endif
}


