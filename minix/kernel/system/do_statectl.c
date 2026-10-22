/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਜੇ USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
ਪੂਰਨ do_statectl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  ਚੋਣ(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  ਹਾਲਤ SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	ਵਾਪਸ(OK);
  ਹਾਲਤ SYS_STATE_ਸੈੱਟ_STATE_ਸਾਰਣੀ:
	/* Set state table for the caller. */
	priv(caller)->s_state_ਸਾਰਣੀ = (vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_krn_sys_statectl.ਪਤਾ;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.ਲੰਮਾਈ;
	ਵਾਪਸ(OK);
  ਹਾਲਤ SYS_STATE_ਜੋੜੋ_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	ਵਾਪਸ ਜੋੜੋ_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_krn_sys_statectl.ਪਤਾ,
	    m_ptr->m_lsys_krn_sys_statectl.ਲੰਮਾਈ);
  ਹਾਲਤ SYS_STATE_ਜੋੜੋ_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	ਵਾਪਸ ਜੋੜੋ_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_krn_sys_statectl.ਪਤਾ,
	    m_ptr->m_lsys_krn_sys_statectl.ਲੰਮਾਈ);
  ਹਾਲਤ SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	ਵਾਪਸ OK;
  ਮੂਲ:
	ਲਿਖੋ("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	ਵਾਪਸ EINVAL;
  }
}

#ਜੇ_ਅੰਤ /* USE_STATECTL */
