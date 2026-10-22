/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_getksig(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  ನೋಂದಣಿ ರಚನೆ proc *rp;

  /* Find the next process with pending signals. */
  ಗಾಗಿ (rp = BEG_USER_ADDR; rp < ಕೊನೆ_PROC_ADDR; rp++) {
      ಆದರೆ (RTS_ISSET(rp, RTS_SIGNALED)) {
          ಆದರೆ (caller->p_endpoint != priv(rp)->s_sig_mgr) ಮುಂದುವರಿ;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (ಶೂನ್ಯ) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          ಹಿಂತಿರುಗಿ(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  ಹಿಂತಿರುಗಿ(OK);
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_GETKSIG */
