/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#అన్తర్భావ "kernel/system.h"

#ఒకవేళ USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
పూర్ణాంకం do_statectl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  ఎంపిక(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  స్థితి SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	తిరిగి(OK);
  స్థితి SYS_STATE_సమితి_STATE_పట్టిక:
	/* Set state table for the caller. */
	priv(caller)->s_state_పట్టిక = (vir_బైట్లు) m_ptr->m_lsys_krn_sys_statectl.చిరునామా;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.నిడివి;
	తిరిగి(OK);
  స్థితి SYS_STATE_కూడు_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	తిరిగి కూడు_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_బైట్లు) m_ptr->m_lsys_krn_sys_statectl.చిరునామా,
	    m_ptr->m_lsys_krn_sys_statectl.నిడివి);
  స్థితి SYS_STATE_కూడు_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	తిరిగి కూడు_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_బైట్లు) m_ptr->m_lsys_krn_sys_statectl.చిరునామా,
	    m_ptr->m_lsys_krn_sys_statectl.నిడివి);
  స్థితి SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	తిరిగి OK;
  అప్రమేయ:
	రాయి("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	తిరిగి EINVAL;
  }
}

#అయితే_ముగింపు /* USE_STATECTL */
