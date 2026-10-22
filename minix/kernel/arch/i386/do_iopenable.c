/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#include "kernel/system.h"
#include <minix/endpoint.h>

#include "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
ကိန်းပြည့် do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  ကိန်းပြည့် proc_nr;

#အကယ်၍ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  အကယ်၍ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } မဟုတ်ပါက အကယ်၍(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ပြန်ပေး(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ပြန်ပေး(OK);
#မဟုတ်ပါက
  ပြန်ပေး(EPERM);
#endif
}


