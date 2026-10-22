/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#ضمن "kernel/system.h"

#ضمن <signal.h>
#ضمن <minix/endpoint.h>

#إذا USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
صحيح do_vtimer(بنية proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  بنية proc *rp;		/* pointer to process the timer belongs to */
  سجل صحيح pt_flag;		/* the misc on/off flag for the req.d timer */
  سجل clock_t *pt_سول;	/* pointer to the process' ticks-left field */ 
  clock_t old_قىممەت;		/* the previous number of ticks left */
  صحيح proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  إذا (! (priv(caller)->s_flags & SYS_PROC)) أرجع(EPERM);

  إذا (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      أرجع(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  إذا (!isokendpt(proc_nr_e, &proc_nr)) أرجع(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  إذا (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_سول = &rp->p_virt_سول;
  } وإلا { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_سول = &rp->p_prof_سول;
  }

  /* Retrieve the old value. */
  إذا (rp->p_misc_flags & pt_flag) {
      old_قىممەت = *pt_سول;
  } وإلا {
      old_قىممەت = 0;
  }

  إذا (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      إذا (m_ptr->VT_قىممەت > 0) {
          *pt_سول = m_ptr->VT_قىممەت;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } وإلا {
          *pt_سول = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_قىممەت = old_قىممەت;

  أرجع(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
فراغ vtimer_check(بنية proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  إذا ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_سول == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_سول = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  إذا ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_سول == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_سول = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
