/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/endpoint.h>

#அந்தர்பாவ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
முழு do_iopenable(கட்டமைப்பு proc * caller, message * m_ptr)
{
  முழு proc_nr;

#எனில் 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  எனில் (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } இல்லையெனில் எனில்(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	திரும்பு(EINVAL);
  enable_iop(proc_addr(proc_nr));
  திரும்பு(OK);
#இல்லையெனில்
  திரும்பு(EPERM);
#endif
}


