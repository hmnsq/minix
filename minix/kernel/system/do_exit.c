/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#include "kernel/system.h"

#include <signal.h>

#အကယ်၍ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ကိန်းပြည့် do_exit(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ကိန်းပြည့် sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ပြန်ပေး(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

