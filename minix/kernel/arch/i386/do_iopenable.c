/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/endpoint.h>

#અન્તર્ભાવ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
પૂર્ણાંક do_iopenable(સંરચના proc * caller, message * m_ptr)
{
  પૂર્ણાંક proc_nr;

#જો 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  જો (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } નહીંતર જો(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	પાછા(EINVAL);
  enable_iop(proc_addr(proc_nr));
  પાછા(OK);
#નહીંતર
  પાછા(EPERM);
#endif
}


