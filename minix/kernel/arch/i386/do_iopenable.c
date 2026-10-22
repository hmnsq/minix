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
ಪೂರ್ಣಾಂಕ do_iopenable(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
  ಪೂರ್ಣಾಂಕ proc_nr;

#ಆದರೆ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ಆದರೆ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ಹಿಂತಿರುಗಿ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ಹಿಂತಿರುಗಿ(OK);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ(EPERM);
#ಆದರೆ_ಅಂತ್ಯ
}


