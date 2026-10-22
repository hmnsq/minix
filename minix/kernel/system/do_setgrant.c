/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_setgrant(ഘടന proc * caller, message * m_ptr)
{
	പൂർണ്ണസംഖ്യ r;

	/* Copy grant table set in priv. struct. */
	എങ്കിൽ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} അല്ലെങ്കിൽ {
		_K_SET_GRANT_TABLE(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.size);
		r = OK;
	}

	തിരികെ r;
}
