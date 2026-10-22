/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಯದಿ USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_statectl(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  ವಿಕಲ್ಪ(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  ಪಕ್ಷ SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	ಪ್ರತ್ಯಾಗಮ(OK);
  ಪಕ್ಷ SYS_STATE_SET_STATE_TABLE:
	/* Set state table for the caller. */
	priv(caller)->s_state_table = (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.length;
	ಪ್ರತ್ಯಾಗಮ(OK);
  ಪಕ್ಷ SYS_STATE_ADD_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	ಪ್ರತ್ಯಾಗಮ add_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  ಪಕ್ಷ SYS_STATE_ADD_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	ಪ್ರತ್ಯಾಗಮ add_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  ಪಕ್ಷ SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	ಪ್ರತ್ಯಾಗಮ OK;
  ಸ್ವಭಾವ:
	printf("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	ಪ್ರತ್ಯಾಗಮ EINVAL;
  }
}

#endif /* USE_STATECTL */
