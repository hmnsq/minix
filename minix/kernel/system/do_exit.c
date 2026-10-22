/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#include "kernel/system.h"

#include <signal.h>

#如果 USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
整数 do_exit(struct proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  整数 sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  返回(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

