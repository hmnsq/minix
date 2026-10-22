/* The kernel call implemented in this file:
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_setgrant.addr    address of grant table in own address space
 *   m_lsys_krn_sys_setgrant.size    number of entries
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_setgrant(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಪೂರ್ಣಾಙ್ಕ r;

	/* Copy grant table set in priv. struct. */
	ಯದಿ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} ಅನ್ಯಥಾ {
		_K_SET_GRANT_TABLE(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.size);
		r = OK;
	}

	ಪ್ರತ್ಯಾಗಮ r;
}
