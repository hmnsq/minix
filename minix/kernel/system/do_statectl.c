/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#include "kernel/system.h"

#如果 USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
整数 do_statectl(struct proc * caller, message * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  switch(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  case SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	返回(OK);
  case SYS_STATE_SET_STATE_TABLE:
	/* Set state table for the caller. */
	priv(caller)->s_state_table = (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.length;
	返回(OK);
  case SYS_STATE_ADD_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	返回 add_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  case SYS_STATE_ADD_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	返回 add_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_bytes) m_ptr->m_lsys_krn_sys_statectl.address,
	    m_ptr->m_lsys_krn_sys_statectl.length);
  case SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	返回 OK;
  default:
	printf("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	返回 EINVAL;
  }
}

#endif /* USE_STATECTL */
