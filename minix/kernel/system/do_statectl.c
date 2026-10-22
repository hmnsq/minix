/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#অন্তর্ভাব "kernel/system.h"

#যদি USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_statectl(কাঠামো proc * caller, message * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  নির্বাচন(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  ক্ষেত্র SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	ফেরত(OK);
  ক্ষেত্র SYS_STATE_SET_STATE_TABLE:
	/* Set state table for the caller. */
	priv(caller)->s_state_table = (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.দৈর্ঘ্য;
	ফেরত(OK);
  ক্ষেত্র SYS_STATE_ADD_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	ফেরত add_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.দৈর্ঘ্য);
  ক্ষেত্র SYS_STATE_ADD_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	ফেরত add_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.দৈর্ঘ্য);
  ক্ষেত্র SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	ফেরত OK;
  পূর্বনির্ধারিত:
	printf("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	ফেরত EINVAL;
  }
}

#endif /* USE_STATECTL */
