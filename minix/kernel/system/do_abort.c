/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਜੇ USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
ਪੂਰਨ do_abort(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  ਪੂਰਨ how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  ਵਾਪਸ(OK);				/* pro-forma (really EDISASTER) */
}

#ਜੇ_ਅੰਤ /* USE_ABORT */

