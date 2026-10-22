/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#شامل "kernel/system.h"
#شامل <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
صحیح do_setgrant(ساخت proc * caller, پیغام * m_ptr)
{
	صحیح r;

	/* Copy grant table set in priv. struct. */
	اگر (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} ورنہ {
		K_سیٹ_GRANT_جدول(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.سائز);
		r = OK;
	}

	واپس r;
}
