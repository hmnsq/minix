/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <signal.h>

#जर USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
पूर्णांक do_exit(संरचना proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  पूर्णांक sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  परत(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

