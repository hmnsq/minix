/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_memset.base	(virtual address)
 *    m_lsys_krn_sys_memset.count	(returns physical address)
 *    m_lsys_krn_sys_memset.pattern	(pattern byte to be written)
 */

#ضمن "kernel/system.h"

#إذا USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
صحيح do_memset(بنية proc * caller, سنیہا * m_ptr)
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  vm_memset(caller, m_ptr->m_lsys_krn_sys_memset.process,
	  m_ptr->m_lsys_krn_sys_memset.ادھار,
	  m_ptr->m_lsys_krn_sys_memset.نمونہ,
	  m_ptr->m_lsys_krn_sys_memset.گنتی);
  أرجع(OK);
}

#جے_انت /* USE_MEMSET */

