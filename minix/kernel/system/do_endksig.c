/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

#ଯଦି USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_endksig(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ଯଦି(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ଫେରନ୍ତୁ EINVAL;

  rp = proc_addr(proc_nr);
  ଯଦି (caller->p_endpoint != priv(rp)->s_sig_mgr) ଫେରନ୍ତୁ(EPERM);
  ଯଦି (!RTS_ISSET(rp, RTS_SIG_PENDING)) ଫେରନ୍ତୁ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ଯଦି (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ଫେରନ୍ତୁ(OK);
}

#endif /* USE_ENDKSIG */

