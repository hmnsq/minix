/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಆದರೆ USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_endksig(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by a signal manager after
 * processing a signal it got with SYS_GETKSIG.
 */
  ನೋಂದಣಿ ರಚನೆ proc *rp;
  ಪೂರ್ಣಾಂಕ proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  ಆದರೆ(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	ಹಿಂತಿರುಗಿ EINVAL;

  rp = proc_addr(proc_nr);
  ಆದರೆ (caller->p_endpoint != priv(rp)->s_sig_mgr) ಹಿಂತಿರುಗಿ(EPERM);
  ಆದರೆ (!RTS_ISSET(rp, RTS_SIG_PENDING)) ಹಿಂತಿರುಗಿ(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  ಆದರೆ (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_ENDKSIG */

