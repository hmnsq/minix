/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <unistd.h>

#જો USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
પૂર્ણાંક do_abort(સંરચના proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  પૂર્ણાંક how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  પાછા(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

