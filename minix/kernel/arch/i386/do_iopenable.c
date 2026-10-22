/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#समावेश "kernel/system.h"
#समावेश <minix/endpoint.h>

#समावेश "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
पूर्णांक do_iopenable(काष्ठा proc * caller, message * m_ptr)
{
  पूर्णांक proc_nr;

#अगर 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  अगर (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } अन्यथा अगर(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	वापस(EINVAL);
  enable_iop(proc_addr(proc_nr));
  वापस(OK);
#अन्यथा
  वापस(EPERM);
#पूर्ण_अगर
}


