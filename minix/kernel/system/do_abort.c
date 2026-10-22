/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#شامل "kernel/system.h"
#شامل <unistd.h>

#اگر USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
صحیح do_abort(ساخت proc * caller, پیغام * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  صحیح how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  واپس(OK);				/* pro-forma (really EDISASTER) */
}

#اگر_ختم /* USE_ABORT */

