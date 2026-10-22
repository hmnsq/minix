/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#अन्तर्भाव "kernel/system.h"

#यदि USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
पूर्णाङ्क do_endksig(संरचना proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  पञ्जिका संरचना proc *rp;
  पूर्णाङ्क proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  यदि(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	फर्काऊ EINVAL;

  rp = proc_addr(proc_nr);
  यदि (caller->p_endpoint != priv(rp)->s_sig_mgr) फर्काऊ(EPERM);
  यदि (!RTS_ISSET(rp, RTS_SIG_PENDING)) फर्काऊ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  यदि (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  फर्काऊ(OK);
}

#endif /* USE_ENDKSIG */

