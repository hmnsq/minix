/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#অন্তর্ভাব "kernel/system.h"

#অন্তর্ভাব <signal.h>

#যদি USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_exit(কাঠামো proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  পূর্ণসংখ্যা sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ফেরত(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

