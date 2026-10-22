/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#include "kernel/system.h"

#include <signal.h>

#ប្រសិនបើ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ចំនួនគត់ do_exit(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ចំនួនគត់ sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ត្រឡប់(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

