/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <signal.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
పూర్ణాంకం do_getksig(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  నమోదు నిర్మాణం proc *rp;

  /* Find the next process with pending signals. */
  కొరకు (rp = BEG_USER_ADDR; rp < చివర_PROC_ADDR; rp++) {
      ఒకవేళ (RTS_ISSET(rp, RTS_SIGNALED)) {
          ఒకవేళ (caller->p_endpoint != priv(rp)->s_sig_mgr) కొనసాగు;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (శూన్య) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          తిరిగి(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  తిరిగి(OK);
}
#అయితే_ముగింపు /* USE_GETKSIG */
