/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#समावेश "kernel/system.h"
#समावेश <signal.h>
#समावेश <minix/endpoint.h>

#अगर USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
पूर्णांक do_getksig(काष्ठा proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  रेजिस्टर काष्ठा proc *rp;

  /* Find the next process with pending signals. */
  क्रम (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      अगर (RTS_ISSET(rp, RTS_SIGNALED)) {
          अगर (caller->p_endpoint != priv(rp)->s_sig_mgr) जारी;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (व्योम) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          वापस(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  वापस(OK);
}
#पूर्ण_अगर /* USE_GETKSIG */
