/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <signal.h>

#எனில் USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
முழு do_exit(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  முழு sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  திரும்பு(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

