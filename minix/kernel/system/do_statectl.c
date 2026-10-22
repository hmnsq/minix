/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#ضمن "kernel/system.h"

#إذا USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
صحيح do_statectl(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  بدل(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  حالة SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	أرجع(OK);
  حالة SYS_STATE_سيٽ_STATE_جدول:
	/* Set state table for the caller. */
	priv(caller)->s_state_جدول = (vir_بائيٽون) m_ptr->m_lsys_krn_sys_statectl.پتو;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.ڊيگهائي;
	أرجع(OK);
  حالة SYS_STATE_جوڙيو_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	أرجع جوڙيو_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_بائيٽون) m_ptr->m_lsys_krn_sys_statectl.پتو,
	    m_ptr->m_lsys_krn_sys_statectl.ڊيگهائي);
  حالة SYS_STATE_جوڙيو_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	أرجع جوڙيو_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_بائيٽون) m_ptr->m_lsys_krn_sys_statectl.پتو,
	    m_ptr->m_lsys_krn_sys_statectl.ڊيگهائي);
  حالة SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	أرجع OK;
  افتراضي:
	لکو("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	أرجع EINVAL;
  }
}

#جيڪڏهن_پورو /* USE_STATECTL */
