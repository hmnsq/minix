/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_memset.base	(virtual address)
 *    m_lsys_krn_sys_memset.count	(returns physical address)
 *    m_lsys_krn_sys_memset.pattern	(pattern byte to be written)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಆದರೆ USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_memset(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  vm_memset(caller, m_ptr->m_lsys_krn_sys_memset.process,
	  m_ptr->m_lsys_krn_sys_memset.ತಳ,
	  m_ptr->m_lsys_krn_sys_memset.ನಮೂನೆ,
	  m_ptr->m_lsys_krn_sys_memset.ಎಣಿಕೆ_2);
  ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_MEMSET */

