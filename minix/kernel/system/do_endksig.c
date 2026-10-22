/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

#если USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
целое do_endksig(struct proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  register struct proc *rp;
  целое proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  если(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	вернуть EINVAL;

  rp = proc_addr(proc_nr);
  если (caller->p_endpoint != priv(rp)->s_sig_mgr) вернуть(EPERM);
  если (!RTS_ISSET(rp, RTS_SIG_PENDING)) вернуть(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  если (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  вернуть(OK);
}

#endif /* USE_ENDKSIG */

