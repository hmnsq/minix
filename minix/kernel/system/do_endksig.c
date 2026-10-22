/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਜੇ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
ਪੂਰਨ do_endksig(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;
  ਪੂਰਨ proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ਜੇ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ਵਾਪਸ EINVAL;

  rp = proc_addr(proc_nr);
  ਜੇ (caller->p_endpoint != priv(rp)->s_sig_mgr) ਵਾਪਸ(EPERM);
  ਜੇ (!RTS_ISSET(rp, RTS_SIG_PENDING)) ਵਾਪਸ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ਜੇ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_ENDKSIG */

