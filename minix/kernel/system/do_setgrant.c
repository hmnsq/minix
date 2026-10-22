/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#ضمن "kernel/system.h"
#ضمن <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
صحيح do_setgrant(بنية proc * caller, سنیہا * m_ptr)
{
	صحيح r;

	/* Copy grant table set in priv. struct. */
	إذا (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} وإلا {
		K_سیٹ_GRANT_جدول(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.ناپ);
		r = OK;
	}

	أرجع r;
}
