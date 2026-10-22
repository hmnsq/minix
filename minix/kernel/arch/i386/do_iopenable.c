/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
ਪੂਰਨ do_iopenable(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਪੂਰਨ proc_nr;

#ਜੇ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ਜੇ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ਨਹੀਂਤਾਂ ਜੇ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ਵਾਪਸ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ਵਾਪਸ(OK);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ(EPERM);
#ਜੇ_ਅੰਤ
}


