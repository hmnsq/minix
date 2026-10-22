/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#অন্তর্ভাব "kernel/system.h"

#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_vtimer(সংরচনা proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  সংরচনা proc *rp;		/* pointer to process the timer belongs to */
  পঞ্জিকা পূর্ণাঙ্ক pt_flag;		/* the misc on/off flag for the req.d timer */
  পঞ্জিকা clock_t *pt_বাম;	/* pointer to the process' ticks-left field */ 
  clock_t old_মান;		/* the previous number of ticks left */
  পূর্ণাঙ্ক proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  যদি (! (priv(caller)->s_flags & SYS_PROC)) প্রত্যাগম(EPERM);

  যদি (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      প্রত্যাগম(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  যদি (!isokendpt(proc_nr_e, &proc_nr)) প্রত্যাগম(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  যদি (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_বাম = &rp->p_virt_বাম;
  } অন্যথা { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_বাম = &rp->p_prof_বাম;
  }

  /* Retrieve the old value. */
  যদি (rp->p_misc_flags & pt_flag) {
      old_মান = *pt_বাম;
  } অন্যথা {
      old_মান = 0;
  }

  যদি (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      যদি (m_ptr->VT_মান > 0) {
          *pt_বাম = m_ptr->VT_মান;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } অন্যথা {
          *pt_বাম = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_মান = old_মান;

  প্রত্যাগম(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
শূন্য vtimer_check(সংরচনা proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  যদি ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_বাম == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_বাম = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  যদি ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_বাম == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_বাম = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
