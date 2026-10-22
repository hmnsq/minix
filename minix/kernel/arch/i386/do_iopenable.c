/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>

#অন্তর্ভাব "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_iopenable(কাঠামো proc * caller, message * m_ptr)
{
  পূর্ণসংখ্যা proc_nr;

#যদি 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  যদি (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } অন্যথা যদি(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ফেরত(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ফেরত(OK);
#অন্যথা
  ফেরত(EPERM);
#endif
}


