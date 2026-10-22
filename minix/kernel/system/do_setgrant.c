/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
ਪੂਰਨ do_setgrant(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਪੂਰਨ r;

	/* Copy grant table set in priv. struct. */
	ਜੇ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} ਨਹੀਂਤਾਂ {
		K_ਸੈੱਟ_GRANT_ਸਾਰਣੀ(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.ਮਾਪ);
		r = OK;
	}

	ਵਾਪਸ r;
}
