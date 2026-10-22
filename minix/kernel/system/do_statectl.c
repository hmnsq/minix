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
صحيح do_statectl(بنية proc * caller, پیغام * m_ptr)
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
  حالة SYS_STATE_سېټ_STATE_جدول:
	/* Set state table for the caller. */
	priv(caller)->s_state_جدول = (vir_بایټونه) m_ptr->m_lsys_krn_sys_statectl.پته;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.اوږدتیا;
	أرجع(OK);
  حالة SYS_STATE_جمع_کړه_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	أرجع جمع_کړه_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_بایټونه) m_ptr->m_lsys_krn_sys_statectl.پته,
	    m_ptr->m_lsys_krn_sys_statectl.اوږدتیا);
  حالة SYS_STATE_جمع_کړه_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	أرجع جمع_کړه_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_بایټونه) m_ptr->m_lsys_krn_sys_statectl.پته,
	    m_ptr->m_lsys_krn_sys_statectl.اوږدتیا);
  حالة SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	أرجع OK;
  افتراضي:
	چاپ_کړه("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	أرجع EINVAL;
  }
}

#د_که_پای /* USE_STATECTL */
