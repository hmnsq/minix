/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/endpoint.h>

#অন্তর্ভাৱ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_iopenable(সংরচনা proc * caller, message * m_ptr)
{
  পূর্ণাঙ্ক proc_nr;

#যদি 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  যদি (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } অন্যথা যদি(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ঘূৰাই(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ঘূৰাই(OK);
#অন্যথা
  ঘূৰাই(EPERM);
#endif
}


