/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#અન્તર્ભાવ "kernel/system.h"

#અન્તર્ભાવ <signal.h>

#જો USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
પૂર્ણાંક do_exit(સંરચના proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  પૂર્ણાંક sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  પાછા(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

