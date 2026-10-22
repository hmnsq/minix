/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#ضمن "kernel/system.h"
#ضمن <unistd.h>

#إذا USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
صحيح do_abort(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  صحيح how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  أرجع(OK);				/* pro-forma (really EDISASTER) */
}

#جيڪڏهن_پورو /* USE_ABORT */

