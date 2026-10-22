/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

#ضمن "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
صحيح do_iopenable(بنية proc * caller, message * m_ptr)
{
  صحيح proc_nr;

#𐤀𐤌 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  𐤀𐤌 (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } وإلا 𐤀𐤌(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	أرجع(EINVAL);
  enable_iop(proc_addr(proc_nr));
  أرجع(OK);
#وإلا
  أرجع(EPERM);
#endif
}


