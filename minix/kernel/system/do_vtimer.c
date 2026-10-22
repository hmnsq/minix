/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಯದಿ USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_vtimer(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  ಸಂರಚನಾ proc *rp;		/* pointer to process the timer belongs to */
  ಪಞ್ಜಿಕಾ ಪೂರ್ಣಾಙ್ಕ pt_flag;		/* the misc on/off flag for the req.d timer */
  ಪಞ್ಜಿಕಾ clock_t *pt_ಎಡ;	/* pointer to the process' ticks-left field */ 
  clock_t old_ಮೌಲ್ಯ;		/* the previous number of ticks left */
  ಪೂರ್ಣಾಙ್ಕ proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  ಯದಿ (! (priv(caller)->s_flags & SYS_PROC)) ಪ್ರತ್ಯಾಗಮ(EPERM);

  ಯದಿ (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      ಪ್ರತ್ಯಾಗಮ(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  ಯದಿ (!isokendpt(proc_nr_e, &proc_nr)) ಪ್ರತ್ಯಾಗಮ(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  ಯದಿ (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_ಎಡ = &rp->p_virt_ಎಡ;
  } ಅನ್ಯಥಾ { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_ಎಡ = &rp->p_prof_ಎಡ;
  }

  /* Retrieve the old value. */
  ಯದಿ (rp->p_misc_flags & pt_flag) {
      old_ಮೌಲ್ಯ = *pt_ಎಡ;
  } ಅನ್ಯಥಾ {
      old_ಮೌಲ್ಯ = 0;
  }

  ಯದಿ (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      ಯದಿ (m_ptr->VT_ಮೌಲ್ಯ > 0) {
          *pt_ಎಡ = m_ptr->VT_ಮೌಲ್ಯ;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } ಅನ್ಯಥಾ {
          *pt_ಎಡ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_ಮೌಲ್ಯ = old_ಮೌಲ್ಯ;

  ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
ಶೂನ್ಯ vtimer_check(ಸಂರಚನಾ proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  ಯದಿ ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_ಎಡ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_ಎಡ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  ಯದಿ ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_ಎಡ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_ಎಡ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
