/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_memset.base	(virtual address)
 *    m_lsys_krn_sys_memset.count	(returns physical address)
 *    m_lsys_krn_sys_memset.pattern	(pattern byte to be written)
 */

#समावेश "kernel/system.h"

#अगर USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
पूर्णांक do_memset(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  vm_memset(caller, m_ptr->m_lsys_krn_sys_memset.process,
	  m_ptr->m_lsys_krn_sys_memset.base,
	  m_ptr->m_lsys_krn_sys_memset.pattern,
	  m_ptr->m_lsys_krn_sys_memset.गिनती);
  वापस(OK);
}

#पूर्ण_अगर /* USE_MEMSET */

