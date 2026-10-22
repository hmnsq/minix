/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#include "kernel/system.h"
#include <signal.h>
#include <minix/endpoint.h>

#якщо USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
ціле do_getksig(struct proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  register struct proc *rp;

  /* Find the next process with pending signals. */
  для (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      якщо (RTS_ISSET(rp, RTS_SIGNALED)) {
          якщо (caller->p_endpoint != priv(rp)->s_sig_mgr) continue;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (void) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          повернути(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  повернути(OK);
}
#endif /* USE_GETKSIG */
