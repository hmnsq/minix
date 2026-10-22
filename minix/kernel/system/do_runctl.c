/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#ضمن "kernel/system.h"
#ضمن <assert.h>

#إذا USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
صحيح do_runctl(بنية proc * caller, پيغام * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  صحيح proc_nr, action, flags;
  سجل بنية proc *rp;

  /* Extract the message parameters and do sanity checking. */
  إذا (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) أرجع(EINVAL);
  إذا (iskerneln(proc_nr)) أرجع(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  إذا (action == RC_STOP && (flags & RC_DELAY)) {
	إذا (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	إذا (rp->p_misc_flags & MF_SIG_DELAY)
		أرجع (EBUSY);
  }

  /* Either set or clear the stop flag. */
  بدل (action) {
  حالة RC_STOP:
#إذا CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  إذا (rp->p_cpu != cpuid) {
		  smp_وقت_جدول_stop_proc(rp);
		  اكسر;
	  }
#جيڪڏهن_پورو
	  RTS_سيٽ(rp, RTS_PROC_STOP);
	اكسر;
  حالة RC_RESUME:
	أكد(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	اكسر;
  افتراضي:
	أرجع(EINVAL);
  }

  أرجع(OK);
}

#جيڪڏهن_پورو /* USE_RUNCTL */

