/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_getksig(ഘടന proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  രജിസ്റ്റർ ഘടന proc *rp;

  /* Find the next process with pending signals. */
  വേണ്ടി (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      എങ്കിൽ (RTS_ISSET(rp, RTS_SIGNALED)) {
          എങ്കിൽ (caller->p_endpoint != priv(rp)->s_sig_mgr) തുടരുക;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (ശൂന്യം) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          തിരികെ(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  തിരികെ(OK);
}
#endif /* USE_GETKSIG */
