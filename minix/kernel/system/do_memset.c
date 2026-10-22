/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_memset.base	(virtual address)
 *    m_lsys_krn_sys_memset.count	(returns physical address)
 *    m_lsys_krn_sys_memset.pattern	(pattern byte to be written)
 */

#అన్తర్భావ "kernel/system.h"

#ఒకవేళ USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
పూర్ణాంకం do_memset(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  vm_memset(caller, m_ptr->m_lsys_krn_sys_memset.process,
	  m_ptr->m_lsys_krn_sys_memset.పునాది,
	  m_ptr->m_lsys_krn_sys_memset.నమూనా,
	  m_ptr->m_lsys_krn_sys_memset.లెక్క);
  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_MEMSET */

