/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#अन्तर्भाव "kernel/system.h"

#जइ USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
पूर्णाङ्क do_statectl(संरचना proc * caller, message * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  विकल्प(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  पक्ष SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	प्रत्यागम(OK);
  पक्ष SYS_STATE_SET_STATE_TABLE:
	/* Set state table for the caller. */
	priv(caller)->s_state_table = (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.length;
	प्रत्यागम(OK);
  पक्ष SYS_STATE_ADD_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	प्रत्यागम add_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  पक्ष SYS_STATE_ADD_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	प्रत्यागम add_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  पक्ष SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	प्रत्यागम OK;
  स्वभाव:
	printf("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	प्रत्यागम EINVAL;
  }
}

#endif /* USE_STATECTL */
