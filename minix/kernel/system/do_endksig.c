/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

#如果 USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
整数 do_endksig(struct proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  register struct proc *rp;
  整数 proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  如果(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	返回 EINVAL;

  rp = proc_addr(proc_nr);
  如果 (caller->p_endpoint != priv(rp)->s_sig_mgr) 返回(EPERM);
  如果 (!RTS_ISSET(rp, RTS_SIG_PENDING)) 返回(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  如果 (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  返回(OK);
}

#endif /* USE_ENDKSIG */

