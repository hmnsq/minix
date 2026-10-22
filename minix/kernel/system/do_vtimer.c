/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#അന്തര്ഭാവ "kernel/system.h"

#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_vtimer(ഘടന proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  ഘടന proc *rp;		/* pointer to process the timer belongs to */
  രജിസ്റ്റർ പൂർണ്ണസംഖ്യ pt_flag;		/* the misc on/off flag for the req.d timer */
  രജിസ്റ്റർ clock_t *pt_ഇടത്;	/* pointer to the process' ticks-left field */ 
  clock_t old_മൂല്യം;		/* the previous number of ticks left */
  പൂർണ്ണസംഖ്യ proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  എങ്കിൽ (! (priv(caller)->s_flags & SYS_PROC)) തിരികെ(EPERM);

  എങ്കിൽ (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      തിരികെ(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  എങ്കിൽ (!isokendpt(proc_nr_e, &proc_nr)) തിരികെ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  എങ്കിൽ (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ഇടത് = &rp->p_virt_ഇടത്;
  } അല്ലെങ്കിൽ { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ഇടത് = &rp->p_prof_ഇടത്;
  }

  /* Retrieve the old value. */
  എങ്കിൽ (rp->p_misc_flags & pt_flag) {
      old_മൂല്യം = *pt_ഇടത്;
  } അല്ലെങ്കിൽ {
      old_മൂല്യം = 0;
  }

  എങ്കിൽ (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      എങ്കിൽ (m_ptr->VT_മൂല്യം > 0) {
          *pt_ഇടത് = m_ptr->VT_മൂല്യം;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } അല്ലെങ്കിൽ {
          *pt_ഇടത് = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_മൂല്യം = old_മൂല്യം;

  തിരികെ(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
ശൂന്യം vtimer_check(ഘടന proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  എങ്കിൽ ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ഇടത് == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ഇടത് = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  എങ്കിൽ ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ഇടത് == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ഇടത് = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
