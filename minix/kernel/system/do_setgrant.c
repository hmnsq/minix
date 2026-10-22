/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
పూర్ణాంకం do_setgrant(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	పూర్ణాంకం r;

	/* Copy grant table set in priv. struct. */
	ఒకవేళ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} లేకపోతే {
		K_సమితి_GRANT_పట్టిక(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.కొలత);
		r = OK;
	}

	తిరిగి r;
}
