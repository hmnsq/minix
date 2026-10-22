/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#ضمن "kernel/system.h"
#ضمن <signal.h>
#ضمن <minix/endpoint.h>

#ܐܢ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
صحيح do_getksig(بنية proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  سجل بنية proc *rp;

  /* Find the next process with pending signals. */
  لكل (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      ܐܢ (RTS_ISSET(rp, RTS_SIGNALED)) {
          ܐܢ (caller->p_endpoint != priv(rp)->s_sig_mgr) استمر;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (فراغ) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          ܦܢܐ(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  ܦܢܐ(OK);
}
#endif /* USE_GETKSIG */
