/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਜੇ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ਪੂਰਨ do_exit(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ਪੂਰਨ sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ਵਾਪਸ(EDONTREPLY);			/* don't reply */
}

#ਜੇ_ਅੰਤ /* USE_EXIT */

