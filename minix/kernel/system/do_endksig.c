/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#അന്തര്ഭാവ "kernel/system.h"

#എങ്കിൽ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_endksig(ഘടന proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  രജിസ്റ്റർ ഘടന proc *rp;
  പൂർണ്ണസംഖ്യ proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  എങ്കിൽ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	തിരികെ EINVAL;

  rp = proc_addr(proc_nr);
  എങ്കിൽ (caller->p_endpoint != priv(rp)->s_sig_mgr) തിരികെ(EPERM);
  എങ്കിൽ (!RTS_ISSET(rp, RTS_SIG_PENDING)) തിരികെ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  എങ്കിൽ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  തിരികെ(OK);
}

#endif /* USE_ENDKSIG */

