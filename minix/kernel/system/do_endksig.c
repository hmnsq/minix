/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

#ถ้า USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
จำนวนเต็ม do_endksig(sa_nracanaa proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  pa_njikaa sa_nracanaa proc *rp;
  จำนวนเต็ม proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ถ้า(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	คืน EINVAL;

  rp = proc_addr(proc_nr);
  ถ้า (caller->p_endpoint != priv(rp)->s_sig_mgr) คืน(EPERM);
  ถ้า (!RTS_ISSET(rp, RTS_SIG_PENDING)) คืน(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ถ้า (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  คืน(OK);
}

#endif /* USE_ENDKSIG */

