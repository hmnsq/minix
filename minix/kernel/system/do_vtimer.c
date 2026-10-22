/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
ਪੂਰਨ do_vtimer(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  ਬਣਤਰ proc *rp;		/* pointer to process the timer belongs to */
  ਰਜਿਸਟਰ ਪੂਰਨ pt_flag;		/* the misc on/off flag for the req.d timer */
  ਰਜਿਸਟਰ clock_t *pt_ਖੱਬਾ;	/* pointer to the process' ticks-left field */ 
  clock_t old_ਮੁੱਲ;		/* the previous number of ticks left */
  ਪੂਰਨ proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  ਜੇ (! (priv(caller)->s_flags & SYS_PROC)) ਵਾਪਸ(EPERM);

  ਜੇ (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      ਵਾਪਸ(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  ਜੇ (!isokendpt(proc_nr_e, &proc_nr)) ਵਾਪਸ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  ਜੇ (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ਖੱਬਾ = &rp->p_virt_ਖੱਬਾ;
  } ਨਹੀਂਤਾਂ { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ਖੱਬਾ = &rp->p_prof_ਖੱਬਾ;
  }

  /* Retrieve the old value. */
  ਜੇ (rp->p_misc_flags & pt_flag) {
      old_ਮੁੱਲ = *pt_ਖੱਬਾ;
  } ਨਹੀਂਤਾਂ {
      old_ਮੁੱਲ = 0;
  }

  ਜੇ (m_ptr->VT_ਸੈੱਟ) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      ਜੇ (m_ptr->VT_ਮੁੱਲ > 0) {
          *pt_ਖੱਬਾ = m_ptr->VT_ਮੁੱਲ;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } ਨਹੀਂਤਾਂ {
          *pt_ਖੱਬਾ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_ਮੁੱਲ = old_ਮੁੱਲ;

  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
ਖਾਲੀ vtimer_check(ਬਣਤਰ proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  ਜੇ ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ਖੱਬਾ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ਖੱਬਾ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  ਜੇ ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ਖੱਬਾ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ਖੱਬਾ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
