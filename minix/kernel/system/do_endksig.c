/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

#ᱡᱩᱫᱤ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
puur_naa_nka do_endksig(sa_nracanaa proc * caller, message * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  pa_njikaa sa_nracanaa proc *rp;
  puur_naa_nka proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ᱡᱩᱫᱤ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ᱨᱩᱣᱟᱹᱲ EINVAL;

  rp = proc_addr(proc_nr);
  ᱡᱩᱫᱤ (caller->p_endpoint != priv(rp)->s_sig_mgr) ᱨᱩᱣᱟᱹᱲ(EPERM);
  ᱡᱩᱫᱤ (!RTS_ISSET(rp, RTS_SIG_PENDING)) ᱨᱩᱣᱟᱹᱲ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ᱡᱩᱫᱤ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ᱨᱩᱣᱟᱹᱲ(OK);
}

#endif /* USE_ENDKSIG */

