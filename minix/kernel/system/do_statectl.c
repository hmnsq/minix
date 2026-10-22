/* The kernel call implemented in this file:
 *   m_type:	SYS_STATECTL
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_statectl.request	(state control request)
 */

#شامل "kernel/system.h"

#اگر USE_STATECTL

/*===========================================================================*
 *			          do_statectl				     *
 *===========================================================================*/
صحیح do_statectl(ساخت proc * caller, پیغام * m_ptr)
{
/* Handle sys_statectl(). A process has issued a state control request. */

  انتخاب(m_ptr->m_lsys_krn_sys_statectl.request)
  {
  حالت SYS_STATE_CLEAR_IPC_REFS:
	/* Clear IPC references for all the processes communicating
	 * with the caller.
	 */
	clear_ipc_refs(caller, EDEADSRCDST);
	واپس(OK);
  حالت SYS_STATE_سیٹ_STATE_جدول:
	/* Set state table for the caller. */
	priv(caller)->s_state_جدول = (vir_بائٹیں) m_ptr->m_lsys_krn_sys_statectl.پتہ;
	priv(caller)->s_state_entries = m_ptr->m_lsys_krn_sys_statectl.لمبائی;
	واپس(OK);
  حالت SYS_STATE_جوڑو_IPC_BL_FILTER:
	/* Add an IPC blacklist filter for the caller. */
	واپس جوڑو_ipc_filter(caller, IPCF_BLACKLIST,
	    (vir_بائٹیں) m_ptr->m_lsys_krn_sys_statectl.پتہ,
	    m_ptr->m_lsys_krn_sys_statectl.لمبائی);
  حالت SYS_STATE_جوڑو_IPC_WL_FILTER:
	/* Add an IPC whitelist filter for the caller. */
	واپس جوڑو_ipc_filter(caller, IPCF_WHITELIST,
	    (vir_بائٹیں) m_ptr->m_lsys_krn_sys_statectl.پتہ,
	    m_ptr->m_lsys_krn_sys_statectl.لمبائی);
  حالت SYS_STATE_CLEAR_IPC_FILTERS:
	/* Clear any IPC filter for the caller. */
	clear_ipc_filters(caller);
	واپس OK;
  طےشدہ:
	لکھو("do_statectl: bad request %d\n",
		m_ptr->m_lsys_krn_sys_statectl.request);
	واپس EINVAL;
  }
}

#اگر_ختم /* USE_STATECTL */
