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
ຈຳນວນເຕັມ do_iopenable(sa_nracanaa proc * caller, message * m_ptr)
{
  ຈຳນວນເຕັມ proc_nr;

#ຖ້າ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ຖ້າ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	ສົ່ງຄືນ(EINVAL);
  enable_iop(proc_addr(proc_nr));
  ສົ່ງຄືນ(OK);
#ບໍ່ດັ່ງນັ້ນ
  ສົ່ງຄືນ(EPERM);
#endif
}


