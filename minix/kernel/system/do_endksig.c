/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#অন্তর্ভাৱ "kernel/system.h"

#যদি USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_endksig(সংরচনা proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  পঞ্জিকা সংরচনা proc *rp;
  পূর্ণাঙ্ক proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  যদি(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ঘূৰাই EINVAL;

  rp = proc_addr(proc_nr);
  যদি (caller->p_endpoint != priv(rp)->s_sig_mgr) ঘূৰাই(EPERM);
  যদি (!RTS_ISSET(rp, RTS_SIG_PENDING)) ঘূৰাই(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  যদি (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ঘূৰাই(OK);
}

#endif /* USE_ENDKSIG */

