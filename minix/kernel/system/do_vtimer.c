/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vtimer(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  ସଂରଚନା proc *rp;		/* pointer to process the timer belongs to */
  ପଞ୍ଜିକରଣ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pt_flag;		/* the misc on/off flag for the req.d timer */
  ପଞ୍ଜିକରଣ clock_t *pt_ବାମ;	/* pointer to the process' ticks-left field */ 
  clock_t old_ମୂଲ୍ୟ;		/* the previous number of ticks left */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  ଯଦି (! (priv(caller)->s_flags & SYS_PROC)) ଫେରନ୍ତୁ(EPERM);

  ଯଦି (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      ଫେରନ୍ତୁ(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  ଯଦି (!isokendpt(proc_nr_e, &proc_nr)) ଫେରନ୍ତୁ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  ଯଦି (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ବାମ = &rp->p_virt_ବାମ;
  } ଅନ୍ୟଥା { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ବାମ = &rp->p_prof_ବାମ;
  }

  /* Retrieve the old value. */
  ଯଦି (rp->p_misc_flags & pt_flag) {
      old_ମୂଲ୍ୟ = *pt_ବାମ;
  } ଅନ୍ୟଥା {
      old_ମୂଲ୍ୟ = 0;
  }

  ଯଦି (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      ଯଦି (m_ptr->VT_ମୂଲ୍ୟ > 0) {
          *pt_ବାମ = m_ptr->VT_ମୂଲ୍ୟ;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } ଅନ୍ୟଥା {
          *pt_ବାମ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_ମୂଲ୍ୟ = old_ମୂଲ୍ୟ;

  ଫେରନ୍ତୁ(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
ଶୂନ୍ୟ vtimer_check(ସଂରଚନା proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  ଯଦି ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ବାମ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ବାମ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  ଯଦି ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ବାମ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ବାମ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
