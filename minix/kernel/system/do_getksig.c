/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
முழு do_getksig(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  பதிவு கட்டமைப்பு proc *rp;

  /* Find the next process with pending signals. */
  க்கு (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      எனில் (RTS_ISSET(rp, RTS_SIGNALED)) {
          எனில் (caller->p_endpoint != priv(rp)->s_sig_mgr) தொடர்;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (வெறுமை) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          திரும்பு(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  திரும்பு(OK);
}
#endif /* USE_GETKSIG */
