/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <signal.h>
#अन्तर्भाव <minix/endpoint.h>

#यदि USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
पूर्णाङ्क do_vtimer(संरचना proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  संरचना proc *rp;		/* pointer to process the timer belongs to */
  पञ्जिका पूर्णाङ्क pt_flag;		/* the misc on/off flag for the req.d timer */
  पञ्जिका clock_t *pt_डावा;	/* pointer to the process' ticks-left field */ 
  clock_t old_मूल्य;		/* the previous number of ticks left */
  पूर्णाङ्क proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  यदि (! (priv(caller)->s_flags & SYS_PROC)) प्रत्यागम(EPERM);

  यदि (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      प्रत्यागम(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  यदि (!isokendpt(proc_nr_e, &proc_nr)) प्रत्यागम(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  यदि (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_डावा = &rp->p_virt_डावा;
  } अन्यथा { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_डावा = &rp->p_prof_डावा;
  }

  /* Retrieve the old value. */
  यदि (rp->p_misc_flags & pt_flag) {
      old_मूल्य = *pt_डावा;
  } अन्यथा {
      old_मूल्य = 0;
  }

  यदि (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      यदि (m_ptr->VT_मूल्य > 0) {
          *pt_डावा = m_ptr->VT_मूल्य;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } अन्यथा {
          *pt_डावा = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_मूल्य = old_मूल्य;

  प्रत्यागम(OK);
}

#endif /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
शून्य vtimer_check(संरचना proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  यदि ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_डावा == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_डावा = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  यदि ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_डावा == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_डावा = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
