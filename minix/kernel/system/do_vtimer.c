/* The kernel call implemented in this file:
 *   m_type:	SYS_VTIMER
 *
 * The parameters for this kernel call are:
 *    m2_i1:	VT_WHICH		(the timer: VT_VIRTUAL or VT_PROF)
 *    m2_i2:	VT_SET			(whether to set, or just retrieve)
 *    m2_l1:	VT_VALUE		(new/old expiration time, in ticks)
 *    m2_l2:	VT_ENDPT		(process to which the timer belongs)
 */

#समावेश "kernel/system.h"

#समावेश <signal.h>
#समावेश <minix/endpoint.h>

#अगर USE_VTIMER

/*===========================================================================*
 *				do_vtimer				     *
 *===========================================================================*/
पूर्णांक do_vtimer(काष्ठा proc * caller, message * m_ptr)
{
/* Set and/or retrieve the value of one of a process' virtual timers. */
  काष्ठा proc *rp;		/* pointer to process the timer belongs to */
  रेजिस्टर पूर्णांक pt_flag;		/* the misc on/off flag for the req.d timer */
  रेजिस्टर घड़ी_प्रकार *pt_बायाँ;	/* pointer to the process' ticks-left field */ 
  घड़ी_प्रकार old_मूल्य;		/* the previous number of ticks left */
  पूर्णांक proc_nr, proc_nr_e;

  /* The requesting process must be privileged. */
  अगर (! (priv(caller)->s_flags & SYS_PROC)) वापस(EPERM);

  अगर (m_ptr->VT_WHICH != VT_VIRTUAL && m_ptr->VT_WHICH != VT_PROF)
      वापस(EINVAL);

  /* The target process must be valid. */
  proc_nr_e = (m_ptr->VT_ENDPT == SELF) ? caller->p_endpoint : m_ptr->VT_ENDPT;
  अगर (!isokendpt(proc_nr_e, &proc_nr)) वापस(EINVAL);
  rp = proc_addr(proc_nr);

  /* Determine which flag and which field in the proc structure we want to
   * retrieve and/or modify. This saves us having to differentiate between
   * VT_VIRTUAL and VT_PROF multiple times below.
   */
  अगर (m_ptr->VT_WHICH == VT_VIRTUAL) {
      pt_flag = MF_VIRT_TIMER;
      pt_बायाँ = &rp->p_virt_बायाँ;
  } अन्यथा { /* VT_PROF */
      pt_flag = MF_PROF_TIMER;
      pt_बायाँ = &rp->p_prof_बायाँ;
  }

  /* Retrieve the old value. */
  अगर (rp->p_misc_flags & pt_flag) {
      old_मूल्य = *pt_बायाँ;
  } अन्यथा {
      old_मूल्य = 0;
  }

  अगर (m_ptr->VT_SET) {
      rp->p_misc_flags &= ~pt_flag;	/* disable virtual timer */

      अगर (m_ptr->VT_मूल्य > 0) {
          *pt_बायाँ = m_ptr->VT_मूल्य;	/* set new timer value */
          rp->p_misc_flags |= pt_flag;	/* (re)enable virtual timer */
      } अन्यथा {
          *pt_बायाँ = 0;			/* clear timer value */
      }
  }

  m_ptr->VT_मूल्य = old_मूल्य;

  वापस(OK);
}

#पूर्ण_अगर /* USE_VTIMER */

/*===========================================================================*
 *				vtimer_check				     *
 *===========================================================================*/
व्योम vtimer_check(काष्ठा proc * rp)
{
  /* This is called from the clock task, so we can be interrupted by the clock
   * interrupt, but not by the system task. Therefore we only have to protect
   * against interference from the clock handler. We can safely perform the
   * following actions without locking as well though, as the clock handler
   * never alters p_misc_flags, and only decreases p_virt_left/p_prof_left.
   */

  /* Check if the virtual timer expired. If so, send a SIGVTALRM signal. */
  अगर ((rp->p_misc_flags & MF_VIRT_TIMER) && rp->p_virt_बायाँ == 0) {
      rp->p_misc_flags &= ~MF_VIRT_TIMER;
      rp->p_virt_बायाँ = 0;
      cause_sig(rp->p_nr, SIGVTALRM);
  }

  /* Check if the profile timer expired. If so, send a SIGPROF signal. */
  अगर ((rp->p_misc_flags & MF_PROF_TIMER) && rp->p_prof_बायाँ == 0) {
      rp->p_misc_flags &= ~MF_PROF_TIMER;
      rp->p_prof_बायाँ = 0;
      cause_sig(rp->p_nr, SIGPROF);
  }
}
