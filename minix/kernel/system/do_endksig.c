/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

#གལ་ཏེ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
ཧྲིལ do_endksig(sa_nracanaa proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  pa_njikaa sa_nracanaa proc *rp;
  ཧྲིལ proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  གལ་ཏེ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ཕྱིར EINVAL;

  rp = proc_addr(proc_nr);
  གལ་ཏེ (caller->p_endpoint != priv(rp)->s_sig_mgr) ཕྱིར(EPERM);
  གལ་ཏེ (!RTS_ISSET(rp, RTS_SIG_PENDING)) ཕྱིར(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  གལ་ཏེ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ཕྱིར(OK);
}

#endif /* USE_ENDKSIG */

