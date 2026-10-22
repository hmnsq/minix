/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <assert.h>

#எனில் USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
முழு do_runctl(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  முழு proc_nr, action, flags;
  பதிவு கட்டமைப்பு proc *rp;

  /* Extract the message parameters and do sanity checking. */
  எனில் (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) திரும்பு(EINVAL);
  எனில் (iskerneln(proc_nr)) திரும்பு(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  எனில் (action == RC_STOP && (flags & RC_DELAY)) {
	எனில் (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	எனில் (rp->p_misc_flags & MF_SIG_DELAY)
		திரும்பு (EBUSY);
  }

  /* Either set or clear the stop flag. */
  தேர்வு (action) {
  நிலை RC_STOP:
#எனில் CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  எனில் (rp->p_cpu != cpuid) {
		  smp_schedule_stop_proc(rp);
		  நிறுத்து;
	  }
#endif
	  RTS_SET(rp, RTS_PROC_STOP);
	நிறுத்து;
  நிலை RC_RESUME:
	ப்ரதிஜ்ஞா(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	நிறுத்து;
  இயல்பு:
	திரும்பு(EINVAL);
  }

  திரும்பு(OK);
}

#endif /* USE_RUNCTL */

