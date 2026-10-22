/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಆದರೆ USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_statectl(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  ಆಯ್ಕೆ(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  ಸ್ಥಿತಿ SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	ಹಿಂತಿರುಗಿ(OK);
  ಸ್ಥಿತಿ SYS_STATE_ಗಣ_STATE_ಕೋಷ್ಟಕ:
	/* Set state table for the caller. */
	priv(caller)->s_state_ಕೋಷ್ಟಕ = (vir_ಬೈಟುಗಳು) m_ptr->m_lsys_krn_sys_statectl.ವಿಳಾಸ;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.ಉದ್ದಳತೆ;
	ಹಿಂತಿರುಗಿ(OK);
  ಸ್ಥಿತಿ SYS_STATE_ಕೂಡಿಸು_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_ಬೈಟುಗಳು) m_ptr->m_lsys_krn_sys_statectl.ವಿಳಾಸ,
	    m_ptr->m_lsys_krn_sys_statectl.ಉದ್ದಳತೆ);
  ಸ್ಥಿತಿ SYS_STATE_ಕೂಡಿಸು_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_ಬೈಟುಗಳು) m_ptr->m_lsys_krn_sys_statectl.ವಿಳಾಸ,
	    m_ptr->m_lsys_krn_sys_statectl.ಉದ್ದಳತೆ);
  ಸ್ಥಿತಿ SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	ಹಿಂತಿರುಗಿ OK;
  ಪೂರ್ವನಿಯೋಜಿತ:
	ಬರೆ("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	ಹಿಂತಿರುಗಿ EINVAL;
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_STATECTL */
