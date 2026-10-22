/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#شامل "kernel/system.h"

#شامل <signal.h>

#اگر USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
صحیح do_exit(ساخت proc * caller, پیغام * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  صحیح sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  واپس(EDONTREPLY);			/* don't reply */
}

#اگر_ختم /* USE_EXIT */

