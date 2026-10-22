/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <signal.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
పూర్ణాంకం do_vtimer(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  నిర్మాణం proc *rp;		/* pointer to process the timer belongs to */
  నమోదు పూర్ణాంకం pt_flag;		/* the misc on/off flag for the req.d timer */
  నమోదు clock_t *pt_ఎడమ;	/* pointer to the process' ticks-left field */ 
  clock_t old_విలువ;		/* the previous number of ticks left */
  పూర్ణాంకం proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  ఒకవేళ (! (priv(caller)->s_flags & SYS_PROC)) తిరిగి(EPERM);

  ఒకవేళ (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      తిరిగి(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  ఒకవేళ (!isokendpt(proc_nr_e, &proc_nr)) తిరిగి(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  ఒకవేళ (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ఎడమ = &rp->p_virt_ఎడమ;
  } లేకపోతే { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ఎడమ = &rp->p_prof_ఎడమ;
  }

  /* Retrieve the old value. */
  ఒకవేళ (rp->p_misc_flags & pt_flag) {
      old_విలువ = *pt_ఎడమ;
  } లేకపోతే {
      old_విలువ = 0;
  }

  ఒకవేళ (m_ptr->VT_సమితి) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      ఒకవేళ (m_ptr->VT_విలువ > 0) {
          *pt_ఎడమ = m_ptr->VT_విలువ;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } లేకపోతే {
          *pt_ఎడమ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_విలువ = old_విలువ;

  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
శూన్య vtimer_check(నిర్మాణం proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  ఒకవేళ ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ఎడమ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ఎడమ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  ఒకవేళ ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ఎడమ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ఎడమ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
