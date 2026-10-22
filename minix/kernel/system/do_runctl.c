/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <assert.h>

#यदि USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
पूर्णाङ्क do_runctl(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  पूर्णाङ्क proc_nr, action, flags;
  पञ्जिका संरचना proc *rp;

  /* Extract the message parameters and do sanity checking. */
  यदि (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) प्रत्यागम(EINVAL);
  यदि (iskerneln(proc_nr)) प्रत्यागम(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  यदि (action == RC_STOP && (flags & RC_DELAY)) {
	यदि (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	यदि (rp->p_misc_flags & MF_SIG_DELAY)
		प्रत्यागम (EBUSY);
  }

  /* Either set or clear the stop flag. */
  विकल्प (action) {
  पक्ष RC_STOP:
#यदि CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  यदि (rp->p_cpu != cpuid) {
		  smp_कालसारणी_stop_proc(rp);
		  भङ्ग;
	  }
#यदि_अन्तः
	  RTS_समुच्चयः(rp, RTS_PROC_STOP);
	भङ्ग;
  पक्ष RC_RESUME:
	प्रतिज्ञा(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	भङ्ग;
  स्वभाव:
	प्रत्यागम(EINVAL);
  }

  प्रत्यागम(OK);
}

#यदि_अन्तः /* USE_RUNCTL */

