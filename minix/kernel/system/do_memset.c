/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_memset.base	(virtual address)
 *    m_lsys_krn_sys_memset.count	(returns physical address)
 *    m_lsys_krn_sys_memset.pattern	(pattern byte to be written)
 */

#अन्तर्भाव "kernel/system.h"

#यदि USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
पूर्णाङ्क do_memset(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  vm_memset(caller, m_ptr->m_lsys_krn_sys_memset.process,
	  m_ptr->m_lsys_krn_sys_memset.आधारः,
	  m_ptr->m_lsys_krn_sys_memset.आदर्शः,
	  m_ptr->m_lsys_krn_sys_memset.गणनम्);
  प्रत्यागम(OK);
}

#यदि_अन्तः /* USE_MEMSET */

