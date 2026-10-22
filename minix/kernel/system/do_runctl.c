/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଯଦି USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_runctl(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr, action, flags;
  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;

  /* Extract the message parameters and do sanity checking. */
  ଯଦି (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) ଫେରନ୍ତୁ(EINVAL);
  ଯଦି (iskerneln(proc_nr)) ଫେରନ୍ତୁ(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  ଯଦି (action == RC_STOP && (flags & RC_DELAY)) {
	ଯଦି (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	ଯଦି (rp->p_misc_flags & MF_SIG_DELAY)
		ଫେରନ୍ତୁ (EBUSY);
  }

  /* Either set or clear the stop flag. */
  ବାଛ (action) {
  ସ୍ଥିତି RC_STOP:
#ଯଦି CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  ଯଦି (rp->p_cpu != cpuid) {
		  smp_schedule_stop_proc(rp);
		  ଭାଙ୍ଗ;
	  }
#endif
	  RTS_SET(rp, RTS_PROC_STOP);
	ଭାଙ୍ଗ;
  ସ୍ଥିତି RC_RESUME:
	ପ୍ରତିଜ୍ଞା(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ(EINVAL);
  }

  ଫେରନ୍ତୁ(OK);
}

#endif /* USE_RUNCTL */

