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

#ಯದಿ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_getksig(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;

  /* Find the next process with pending signals. */
  ಪ್ರತಿ (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      ಯದಿ (RTS_ISSET(rp, RTS_SIGNALED)) {
          ಯದಿ (caller->p_endpoint != priv(rp)->s_sig_mgr) ಅನುವರ್ತ;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (ಶೂನ್ಯ) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          ಪ್ರತ್ಯಾಗಮ(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  ಪ್ರತ್ಯಾಗಮ(OK);
}
#endif /* USE_GETKSIG */
