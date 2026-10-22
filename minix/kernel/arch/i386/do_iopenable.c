/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

#अन्तर्भाव "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
पूर्णाङ्क do_iopenable(संरचना proc * caller, message * m_ptr)
{
  पूर्णाङ्क proc_nr;

#सचे 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  सचे (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } अञ्ञथा सचे(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	पटिनिवत्त(EINVAL);
  enable_iop(proc_addr(proc_nr));
  पटिनिवत्त(OK);
#अञ्ञथा
  पटिनिवत्त(EPERM);
#endif
}


