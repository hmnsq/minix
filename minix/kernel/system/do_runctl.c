/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <assert.h>

#ఒకవేళ USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
పూర్ణాంకం do_runctl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  పూర్ణాంకం proc_nr, action, flags;
  నమోదు నిర్మాణం proc *rp;

  /* Extract the message parameters and do sanity checking. */
  ఒకవేళ (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) తిరిగి(EINVAL);
  ఒకవేళ (iskerneln(proc_nr)) తిరిగి(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  ఒకవేళ (action == RC_STOP && (flags & RC_DELAY)) {
	ఒకవేళ (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	ఒకవేళ (rp->p_misc_flags & MF_SIG_DELAY)
		తిరిగి (EBUSY);
  }

  /* Either set or clear the stop flag. */
  ఎంపిక (action) {
  స్థితి RC_STOP:
#ఒకవేళ CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  ఒకవేళ (rp->p_cpu != cpuid) {
		  smp_కాలపట్టిక_stop_proc(rp);
		  ఆపు;
	  }
#అయితే_ముగింపు
	  RTS_సమితి(rp, RTS_PROC_STOP);
	ఆపు;
  స్థితి RC_RESUME:
	ప్రతిజ్ఞా(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	ఆపు;
  అప్రమేయ:
	తిరిగి(EINVAL);
  }

  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_RUNCTL */

