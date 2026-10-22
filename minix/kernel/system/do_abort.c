/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <unistd.h>

#எனில் USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
முழு do_abort(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  முழு how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  திரும்பு(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

