/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <assert.h>

#എങ്കിൽ USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_runctl(ഘടന proc * caller, message * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  പൂർണ്ണസംഖ്യ proc_nr, action, flags;
  രജിസ്റ്റർ ഘടന proc *rp;

  /* Extract the message parameters and do sanity checking. */
  എങ്കിൽ (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) തിരികെ(EINVAL);
  എങ്കിൽ (iskerneln(proc_nr)) തിരികെ(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  എങ്കിൽ (action == RC_STOP && (flags & RC_DELAY)) {
	എങ്കിൽ (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	എങ്കിൽ (rp->p_misc_flags & MF_SIG_DELAY)
		തിരികെ (EBUSY);
  }

  /* Either set or clear the stop flag. */
  തിരഞ്ഞെടുപ്പ് (action) {
  അവസ്ഥ RC_STOP:
#എങ്കിൽ CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  എങ്കിൽ (rp->p_cpu != cpuid) {
		  smp_schedule_stop_proc(rp);
		  നിർത്തുക;
	  }
#endif
	  RTS_SET(rp, RTS_PROC_STOP);
	നിർത്തുക;
  അവസ്ഥ RC_RESUME:
	പ്രതിജ്ഞാ(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
	തിരികെ(EINVAL);
  }

  തിരികെ(OK);
}

#endif /* USE_RUNCTL */

