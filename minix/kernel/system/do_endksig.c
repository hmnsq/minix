/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#అన్తర్భావ "kernel/system.h"

#ఒకవేళ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
పూర్ణాంకం do_endksig(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  నమోదు నిర్మాణం proc *rp;
  పూర్ణాంకం proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ఒకవేళ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	తిరిగి EINVAL;

  rp = proc_addr(proc_nr);
  ఒకవేళ (caller->p_endpoint != priv(rp)->s_sig_mgr) తిరిగి(EPERM);
  ఒకవేళ (!RTS_ISSET(rp, RTS_SIG_PENDING)) తిరిగి(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ఒకవేళ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_ENDKSIG */

