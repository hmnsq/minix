/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#অন্তর্ভাৱ "kernel/system.h"

#অন্তর্ভাৱ <signal.h>

#যদি USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_exit(সংরচনা proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  পূর্ণাঙ্ক sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ঘূৰাই(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

