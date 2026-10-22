/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#ضمن "kernel/system.h"

#𐤀𐤌 USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
صحيح do_endksig(بنية proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  سجل بنية proc *rp;
  صحيح proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  𐤀𐤌(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	أرجع EINVAL;

  rp = proc_addr(proc_nr);
  𐤀𐤌 (caller->p_endpoint != priv(rp)->s_sig_mgr) أرجع(EPERM);
  𐤀𐤌 (!RTS_ISSET(rp, RTS_SIG_PENDING)) أرجع(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  𐤀𐤌 (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  أرجع(OK);
}

#endif /* USE_ENDKSIG */

