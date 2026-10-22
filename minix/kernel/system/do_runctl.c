/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <assert.h>

#যদি USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_runctl(সংরচনা proc * caller, message * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  পূর্ণাঙ্ক proc_nr, action, flags;
  পঞ্জিকা সংরচনা proc *rp;

  /* Extract the message parameters and do sanity checking. */
  যদি (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) ঘূৰাই(EINVAL);
  যদি (iskerneln(proc_nr)) ঘূৰাই(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  যদি (action == RC_STOP && (flags & RC_DELAY)) {
	যদি (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	যদি (rp->p_misc_flags & MF_SIG_DELAY)
		ঘূৰাই (EBUSY);
  }

  /* Either set or clear the stop flag. */
  ৱিকল্প (action) {
  পক্ষ RC_STOP:
#যদি CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  যদি (rp->p_cpu != cpuid) {
		  smp_schedule_stop_proc(rp);
		  ভঙ্গ;
	  }
#endif
	  RTS_SET(rp, RTS_PROC_STOP);
	ভঙ্গ;
  পক্ষ RC_RESUME:
	প্রতিজ্ঞা(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	ভঙ্গ;
  স্ৱভাৱ:
	ঘূৰাই(EINVAL);
  }

  ঘূৰাই(OK);
}

#endif /* USE_RUNCTL */

