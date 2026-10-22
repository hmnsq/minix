/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#شامل "kernel/system.h"

#شامل <signal.h>
#شامل <minix/endpoint.h>

#اگر USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
صحیح do_vtimer(ساختار proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  ساختار proc *rp;		/* pointer to process the timer belongs to */
  ثبات صحیح pt_flag;		/* the misc on/off flag for the req.d timer */
  ثبات clock_t *pt_چپ;	/* pointer to the process' ticks-left field */ 
  clock_t old_مقدار;		/* the previous number of ticks left */
  صحیح proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  اگر (! (priv(caller)->s_flags & SYS_PROC)) بازگشت(EPERM);

  اگر (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      بازگشت(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  اگر (!isokendpt(proc_nr_e, &proc_nr)) بازگشت(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  اگر (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_چپ = &rp->p_virt_چپ;
  } وگرنه { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_چپ = &rp->p_prof_چپ;
  }

  /* Retrieve the old value. */
  اگر (rp->p_misc_flags & pt_flag) {
      old_مقدار = *pt_چپ;
  } وگرنه {
      old_مقدار = 0;
  }

  اگر (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      اگر (m_ptr->VT_مقدار > 0) {
          *pt_چپ = m_ptr->VT_مقدار;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } وگرنه {
          *pt_چپ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_مقدار = old_مقدار;

  بازگشت(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
تهی vtimer_check(ساختار proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  اگر ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_چپ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_چپ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  اگر ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_چپ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_چپ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
