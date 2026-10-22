/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#include "kernel/system.h"
#include <unistd.h>

#ᬬᬾᬦ᭄ USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
puur_naa_nka do_abort(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  puur_naa_nka how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  ᬯᬮᬶ(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

