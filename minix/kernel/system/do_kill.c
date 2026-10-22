/* The kernel call that is implemented in this file:
 *	m_type: SYS_KILL
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt  	# process to signal/ pending
 *	m_sigcalls.sig		# signal number to send to process
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <signal.h>

#ఒకవేళ USE_KILL

/*===========================================================================*
 *			          do_kill				     *
 *===========================================================================*/
పూర్ణాంకం do_kill(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_kill(). Cause a signal to be sent to a process. Any request
 * is added to the map of pending signals and the signal manager
 * associated to the process is informed about the new signal. The signal
 * is then delivered using POSIX signal handlers for user processes, or
 * translated into an IPC message for system services.
 */
  proc_nr_t proc_nr, proc_nr_e;
  పూర్ణాంకం sig_nr = m_ptr->m_sigcalls.sig;

  proc_nr_e = (proc_nr_t)m_ptr->m_sigcalls.endpt;

  ఒకవేళ (!isokendpt(proc_nr_e, &proc_nr)) తిరిగి(EINVAL);
  ఒకవేళ (sig_nr >= _NSIG) తిరిగి(EINVAL);
  ఒకవేళ (iskerneln(proc_nr)) తిరిగి(EPERM);

  /* Set pending signal to be processed by the signal manager. */
  cause_sig(proc_nr, sig_nr);

  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_KILL */

