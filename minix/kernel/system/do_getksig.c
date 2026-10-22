/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <signal.h>
#अन्तर्भाव <minix/endpoint.h>

#जइ USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
पूर्णाङ्क do_getksig(संरचना proc * caller, message * m_ptr)
{
/* The signal manager is ready to accept signals and repeatedly does a kernel
 * call to get one. Find a process with pending signals. If no signals are
 * available, return NONE in the process number field.
 */
  पञ्जिका संरचना proc *rp;

  /* Find the next process with pending signals. */
  प्रति (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      जइ (RTS_ISSET(rp, RTS_SIGNALED)) {
          जइ (caller->p_endpoint != priv(rp)->s_sig_mgr) अनुवर्त;
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          (शून्य) sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          प्रत्यागम(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  प्रत्यागम(OK);
}
#endif /* USE_GETKSIG */
