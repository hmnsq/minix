/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#അന്തര്ഭാവ "kernel/system.h"

#അന്തര്ഭാവ <signal.h>

#എങ്കിൽ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_exit(ഘടന proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  പൂർണ്ണസംഖ്യ sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  തിരികെ(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

