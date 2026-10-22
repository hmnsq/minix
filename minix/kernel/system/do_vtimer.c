/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#અન્તર્ભાવ "kernel/system.h"

#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
પૂર્ણાંક do_vtimer(સંરચના proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  સંરચના proc *rp;		/* pointer to process the timer belongs to */
  નોંધણી પૂર્ણાંક pt_flag;		/* the misc on/off flag for the req.d timer */
  નોંધણી clock_t *pt_ડાબું;	/* pointer to the process' ticks-left field */ 
  clock_t old_કિંમત;		/* the previous number of ticks left */
  પૂર્ણાંક proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  જો (! (priv(caller)->s_flags & SYS_PROC)) પાછા(EPERM);

  જો (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      પાછા(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  જો (!isokendpt(proc_nr_e, &proc_nr)) પાછા(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  જો (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ડાબું = &rp->p_virt_ડાબું;
  } નહીંતર { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ડાબું = &rp->p_prof_ડાબું;
  }

  /* Retrieve the old value. */
  જો (rp->p_misc_flags & pt_flag) {
      old_કિંમત = *pt_ડાબું;
  } નહીંતર {
      old_કિંમત = 0;
  }

  જો (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      જો (m_ptr->VT_કિંમત > 0) {
          *pt_ડાબું = m_ptr->VT_કિંમત;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } નહીંતર {
          *pt_ડાબું = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_કિંમત = old_કિંમત;

  પાછા(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
શૂન્ય vtimer_check(સંરચના proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  જો ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ડાબું == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ડાબું = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  જો ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ડાબું == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ડાબું = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
