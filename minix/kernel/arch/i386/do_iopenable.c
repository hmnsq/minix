/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_iopenable(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  ಪೂರ್ಣಾಙ್ಕ proc_nr;

#ಯದಿ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ಯದಿ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ಅನ್ಯಥಾ ಯದಿ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ಪ್ರತ್ಯಾಗಮ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ಪ್ರತ್ಯಾಗಮ(OK);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ(EPERM);
#endif
}


