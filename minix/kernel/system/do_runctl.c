/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
ਪੂਰਨ do_runctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  ਪੂਰਨ proc_nr, action, flags;
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;

  /* Extract the message parameters and do sanity checking. */
  ਜੇ (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) ਵਾਪਸ(EINVAL);
  ਜੇ (iskerneln(proc_nr)) ਵਾਪਸ(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  ਜੇ (action == RC_STOP && (flags & RC_DELAY)) {
	ਜੇ (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	ਜੇ (rp->p_misc_flags & MF_SIG_DELAY)
		ਵਾਪਸ (EBUSY);
  }

  /* Either set or clear the stop flag. */
  ਚੋਣ (action) {
  ਹਾਲਤ RC_STOP:
#ਜੇ CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  ਜੇ (rp->p_cpu != cpuid) {
		  smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc(rp);
		  ਰੋਕੋ;
	  }
#ਜੇ_ਅੰਤ
	  RTS_ਸੈੱਟ(rp, RTS_PROC_STOP);
	ਰੋਕੋ;
  ਹਾਲਤ RC_RESUME:
	ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	ਰੋਕੋ;
  ਮੂਲ:
	ਵਾਪਸ(EINVAL);
  }

  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_RUNCTL */

