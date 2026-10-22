/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#include "kernel/system.h"
#include <unistd.h>

#གལ་ཏེ USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
ཧྲིལ do_abort(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  ཧྲིལ how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  ཕྱིར(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

