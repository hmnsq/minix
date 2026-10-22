/* The kernel call that is implemented in this file:
 *	m_type: SYS_KILL
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt  	# process to signal/ pending
 *	m_sigcalls.sig		# signal number to send to process
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <signal.h>

#எனில் USE_KILL

/*===========================================================================*
 *			          do_kill				     *
 *===========================================================================*/
முழு do_kill(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Handle sys_kill(). Cause a signal to be sent to a process. Any request
 * is added to the map of pending signals and the signal manager
 * associated to the process is informed about the new signal. The signal
 * is then delivered using POSIX signal handlers for user processes, or
 * translated into an IPC message for system services.
 */
  proc_nr_t proc_nr, proc_nr_e;
  முழு sig_nr = m_ptr->m_sigcalls.sig;

  proc_nr_e = (proc_nr_t)m_ptr->m_sigcalls.endpt;

  எனில் (!isokendpt(proc_nr_e, &proc_nr)) திரும்பு(EINVAL);
  எனில் (sig_nr >= _NSIG) திரும்பு(EINVAL);
  எனில் (iskerneln(proc_nr)) திரும்பு(EPERM);

  /* Set pending signal to be processed by the signal manager. */
  cause_sig(proc_nr, sig_nr);

  திரும்பு(OK);
}

#endif /* USE_KILL */

