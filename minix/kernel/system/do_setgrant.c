/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_setgrant(কাঠামো proc * caller, message * m_ptr)
{
	পূর্ণসংখ্যা r;

	/* Copy grant table set in priv. struct. */
	যদি (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} অন্যথা {
		_K_SET_GRANT_TABLE(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.আকার_2);
		r = OK;
	}

	ফেরত r;
}
