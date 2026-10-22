/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಯದಿ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_endksig(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;
  ಪೂರ್ಣಾಙ್ಕ proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ಯದಿ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ಪ್ರತ್ಯಾಗಮ EINVAL;

  rp = proc_addr(proc_nr);
  ಯದಿ (caller->p_endpoint != priv(rp)->s_sig_mgr) ಪ್ರತ್ಯಾಗಮ(EPERM);
  ಯದಿ (!RTS_ISSET(rp, RTS_SIG_PENDING)) ಪ್ರತ್ಯಾಗಮ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ಯದಿ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif /* USE_ENDKSIG */

