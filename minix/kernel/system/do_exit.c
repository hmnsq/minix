/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#ضمن "kernel/system.h"

#ضمن <signal.h>

#إذا USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
صحيح do_exit(بنية proc * caller, سنیہا * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  صحيح sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  أرجع(EDONTREPLY);			/* don't reply */
}

#جے_انت /* USE_EXIT */

