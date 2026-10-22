/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#include "kernel/system.h"
#include <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
цео do_setgrant(struct proc * caller, message * m_ptr)
{
	цео r;

	/* Copy grant table set in priv. struct. */
	ако (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} инако {
		_K_SET_GRANT_TABLE(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.size);
		r = OK;
	}

	врати r;
}
