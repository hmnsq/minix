/* The kernel call that is implemented in this file:
 *	m_type: SYS_KILL
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt  	# process to signal/ pending
 *	m_sigcalls.sig		# signal number to send to process
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <signal.h>

#यदि USE_KILL

/*===========================================================================*
 *			          do_kill				     *
 *===========================================================================*/
पूर्णाङ्क do_kill(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Handle sys_kill(). Cause a signal to be sent to a process. Any request
 * is added to the map of pending signals and the signal manager
 * associated to the process is informed about the new signal. The signal
 * is then delivered using POSIX signal handlers for user processes, or
 * translated into an IPC message for system services.
 */
  proc_nr_t proc_nr, proc_nr_e;
  पूर्णाङ्क sig_nr = m_ptr->m_sigcalls.sig;

  proc_nr_e = (proc_nr_t)m_ptr->m_sigcalls.endpt;

  यदि (!isokendpt(proc_nr_e, &proc_nr)) प्रत्यागम(EINVAL);
  यदि (sig_nr >= _NSIG) प्रत्यागम(EINVAL);
  यदि (iskerneln(proc_nr)) प्रत्यागम(EPERM);

  /* Set pending signal to be processed by the signal manager. */
  cause_sig(proc_nr, sig_nr);

  प्रत्यागम(OK);
}

#यदि_अन्तः /* USE_KILL */

