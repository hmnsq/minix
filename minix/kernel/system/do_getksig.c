/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getksig(ସଂରଚନା proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;

  /* Find the next process with pending signals. */
  ପାଇଁ (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      ଯଦି (RTS_ISSET(rp, RTS_SIGNALED)) {
          ଯଦି (caller->p_endpoint != priv(rp)->s_sig_mgr) ଜାରି;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (ଶୂନ୍ୟ) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          ଫେରନ୍ତୁ(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  ଫେରନ୍ତୁ(OK);
}
#endif /* USE_GETKSIG */
