/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_getksig(কাঠামো proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  নিবন্ধ কাঠামো proc *rp;

  /* Find the next process with pending signals. */
  জন্য (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      যদি (RTS_ISSET(rp, RTS_SIGNALED)) {
          যদি (caller->p_endpoint != priv(rp)->s_sig_mgr) চালু;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (শূন্য) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          ফেরত(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  ফেরত(OK);
}
#endif /* USE_GETKSIG */
