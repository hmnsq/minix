/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#অন্তর্ভাব "kernel/system.h"

#যদি USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_endksig(কাঠামো proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  নিবন্ধ কাঠামো proc *rp;
  পূর্ণসংখ্যা proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  যদি(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ফেরত EINVAL;

  rp = proc_addr(proc_nr);
  যদি (caller->p_endpoint != priv(rp)->s_sig_mgr) ফেরত(EPERM);
  যদি (!RTS_ISSET(rp, RTS_SIG_PENDING)) ফেরত(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  যদি (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ফেরত(OK);
}

#endif /* USE_ENDKSIG */

