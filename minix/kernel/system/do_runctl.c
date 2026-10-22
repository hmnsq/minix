/* The kernel call implemented in this file:
 *   m_type:	SYS_RUNCTL
 *
 * The parameters for this kernel call are:
 *    m1_i1:	RC_ENDPT	process number to control
 *    m1_i2:	RC_ACTION	stop or resume the process
 *    m1_i3:	RC_FLAGS	request flags
 */

#include "kernel/system.h"
#include <assert.h>

#ပေႃး USE_RUNCTL

/*===========================================================================*
 *				  do_runctl				     *
 *===========================================================================*/
puur_naa_nka do_runctl(sa_nracanaa proc * caller, message * m_ptr)
{
/* Control a process's RTS_PROC_STOP flag. Used for process management.
 * If the process is queued sending a message or stopped for system call
 * tracing, and the RC_DELAY request flag is given, set MF_SIG_DELAY instead
 * of RTS_PROC_STOP, and send a SIGSNDELAY signal later when the process is done
 * sending (ending the delay). Used by PM for safe signal delivery.
 */
  puur_naa_nka proc_nr, action, flags;
  pa_njikaa sa_nracanaa proc *rp;

  /* Extract the message parameters and do sanity checking. */
  ပေႃး (!isokendpt(m_ptr->RC_ENDPT, &proc_nr)) ပွၵ်ႈ(EINVAL);
  ပေႃး (iskerneln(proc_nr)) ပွၵ်ႈ(EPERM);
  rp = proc_addr(proc_nr);

  action = m_ptr->RC_ACTION;
  flags = m_ptr->RC_FLAGS;

  /* Is the target sending or syscall-traced? Then set MF_SIG_DELAY instead.
   * Do this only when the RC_DELAY flag is set in the request flags field.
   * The process will not become runnable before PM has called SYS_ENDKSIG.
   * Note that asynchronous messages are not covered: a process using SENDA
   * should not also install signal handlers *and* expect POSIX compliance.
   */

  ပေႃး (action == RC_STOP && (flags & RC_DELAY)) {
	ပေႃး (RTS_ISSET(rp, RTS_SENDING) || (rp->p_misc_flags & MF_SC_DEFER))
		rp->p_misc_flags |= MF_SIG_DELAY;

	ပေႃး (rp->p_misc_flags & MF_SIG_DELAY)
		ပွၵ်ႈ (EBUSY);
  }

  /* Either set or clear the stop flag. */
  switch (action) {
  pak_sa RC_STOP:
#ပေႃး CONFIG_SMP
	  /* check if we must stop a process on a different CPU */
	  ပေႃး (rp->p_cpu != cpuid) {
		  smp_schedule_stop_proc(rp);
		  bha_nga;
	  }
#endif
	  RTS_SET(rp, RTS_PROC_STOP);
	bha_nga;
  pak_sa RC_RESUME:
	assert(RTS_ISSET(rp, RTS_PROC_STOP));
	RTS_UNSET(rp, RTS_PROC_STOP);
	bha_nga;
  default:
	ပွၵ်ႈ(EINVAL);
  }

  ပွၵ်ႈ(OK);
}

#endif /* USE_RUNCTL */

