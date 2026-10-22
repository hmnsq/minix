/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#அந்தர்பாவ "kernel/system.h"

#எனில் USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
முழு do_endksig(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  பதிவு கட்டமைப்பு proc *rp;
  முழு proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  எனில்(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	திரும்பு EINVAL;

  rp = proc_addr(proc_nr);
  எனில் (caller->p_endpoint != priv(rp)->s_sig_mgr) திரும்பு(EPERM);
  எனில் (!RTS_ISSET(rp, RTS_SIG_PENDING)) திரும்பு(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  எனில் (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  திரும்பு(OK);
}

#endif /* USE_ENDKSIG */

