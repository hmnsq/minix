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
ಪೂರ್ಣಾಂಕ do_setgrant(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ಪೂರ್ಣಾಂಕ r;

	/* Copy grant table set in priv. struct. */
	ಆದರೆ (RTS_ISSET(caller, RTS_NO_PRIV) || !(priv(caller))) {
		r = EPERM;
	} ಇಲ್ಲದಿದ್ದರೆ {
		K_ಗಣ_GRANT_ಕೋಷ್ಟಕ(caller,
			m_ptr->m_lsys_krn_sys_setgrant.addr,
			m_ptr->m_lsys_krn_sys_setgrant.ಗಾತ್ರ_2);
		r = OK;
	}

	ಹಿಂತಿರುಗಿ r;
}
