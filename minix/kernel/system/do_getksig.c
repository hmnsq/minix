/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
પૂર્ણાંક do_getksig(સંરચના proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  નોંધણી સંરચના proc *rp;

  /* Find the next process with pending signals. */
  માટે (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      જો (RTS_ISSET(rp, RTS_SIGNALED)) {
          જો (caller->p_endpoint != priv(rp)->s_sig_mgr) ચાલુ;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (શૂન્ય) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          પાછા(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  પાછા(OK);
}
#endif /* USE_GETKSIG */
