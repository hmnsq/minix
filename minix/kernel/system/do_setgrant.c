/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
पूर्णाङ्क do_setgrant(संरचना proc * caller, message * m_ptr)
{
	पूर्णाङ्क r;

	/* Copy grant table set in priv. struct. */
	जइ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} अण्णहा {
		_K_SET_GRANT_TABLE(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.size);
		r = OK;
	}

	प्रत्यागम r;
}
