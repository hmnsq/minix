/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
முழு do_vtimer(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  கட்டமைப்பு proc *rp;		/* pointer to process the timer belongs to */
  பதிவு முழு pt_flag;		/* the misc on/off flag for the req.d timer */
  பதிவு clock_t *pt_இடது;	/* pointer to the process' ticks-left field */ 
  clock_t old_மதிப்பு;		/* the previous number of ticks left */
  முழு proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  எனில் (! (priv(caller)->s_flags & SYS_PROC)) திரும்பு(EPERM);

  எனில் (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      திரும்பு(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  எனில் (!isokendpt(proc_nr_e, &proc_nr)) திரும்பு(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  எனில் (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_இடது = &rp->p_virt_இடது;
  } இல்லையெனில் { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_இடது = &rp->p_prof_இடது;
  }

  /* Retrieve the old value. */
  எனில் (rp->p_misc_flags & pt_flag) {
      old_மதிப்பு = *pt_இடது;
  } இல்லையெனில் {
      old_மதிப்பு = 0;
  }

  எனில் (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      எனில் (m_ptr->VT_மதிப்பு > 0) {
          *pt_இடது = m_ptr->VT_மதிப்பு;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } இல்லையெனில் {
          *pt_இடது = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_மதிப்பு = old_மதிப்பு;

  திரும்பு(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
வெறுமை vtimer_check(கட்டமைப்பு proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  எனில் ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_இடது == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_இடது = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  எனில் ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_இடது == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_இடது = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
