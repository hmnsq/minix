/* The system call implemented in this file:
 *   m_type:	SYS_IOPENABLE
 *
 * The parameters for this system call are:
 *   m_lsys_krn_sys_iopenable.endpt	(process to give I/O Protection Level bits)
 *
 * Author:
 *    Jorrit N. Herder <jnherder@cs.vu.nl>
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/endpoint.h>

#అన్తర్భావ "arch_proto.h"

/*===========================================================================*
 *			        do_iopenable				     *
 *===========================================================================*/
పూర్ణాంకం do_iopenable(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  పూర్ణాంకం proc_nr;

#ఒకవేళ 1 /* ENABLE_USERPRIV && ENABLE_USERIOPL */
  ఒకవేళ (m_ptr->m_lsys_krn_sys_iopenable.endpt == SELF) {
	okendpt(caller->p_endpoint, &proc_nr);
  } లేకపోతే ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_iopenable.endpt, &proc_nr))
	తిరిగి(EINVAL);
  enable_iop(proc_addr(proc_nr));
  తిరిగి(OK);
#లేకపోతే
  తిరిగి(EPERM);
#అయితే_ముగింపు
}


