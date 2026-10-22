/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#شامل "kernel/system.h"
#شامل <signal.h>
#شامل <minix/endpoint.h>

#اگر USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
صحیح do_getksig(ساخت proc * caller, پیغام * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  اندراج ساخت proc *rp;

  /* Find the next process with pending signals. */
  برائے (rp = BEG_USER_ADDR; rp < انتہا_PROC_ADDR; rp++) {
      اگر (RTS_ISSET(rp, RTS_SIGNALED)) {
          اگر (caller->p_endpoint != priv(rp)->s_sig_mgr) جاری;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (خالی) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          واپس(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  واپس(OK);
}
#اگر_ختم /* USE_GETKSIG */
