/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/endpoint.h>

#അന്തര്ഭാവ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_iopenable(ഘടന proc * caller, message * m_ptr)
{
  പൂർണ്ണസംഖ്യ proc_nr;

#എങ്കിൽ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  എങ്കിൽ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } അല്ലെങ്കിൽ എങ്കിൽ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	തിരികെ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  തിരികെ(OK);
#അല്ലെങ്കിൽ
  തിരികെ(EPERM);
#endif
}


