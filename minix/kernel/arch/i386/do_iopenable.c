/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#شامل "kernel/system.h"
#شامل <minix/endpoint.h>

#شامل "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
صحیح do_iopenable(ساخت proc * caller, پیغام * m_ptr)
{
  صحیح proc_nr;

#اگر 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  اگر (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ورنہ اگر(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	واپس(EINVAL);
  enable_iop(proc_addr(proc_nr));
  واپس(OK);
#ورنہ
  واپس(EPERM);
#اگر_ختم
}


