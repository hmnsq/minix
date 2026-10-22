/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

#अन्तर्भाव "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
पूर्णाङ्क do_iopenable(संरचना proc * caller, सन्देशः * m_ptr)
{
  पूर्णाङ्क proc_nr;

#यदि 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  यदि (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } अन्यथा यदि(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	प्रत्यागम(EINVAL);
  enable_iop(proc_addr(proc_nr));
  प्रत्यागम(OK);
#अन्यथा
  प्रत्यागम(EPERM);
#यदि_अन्तः
}


