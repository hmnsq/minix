/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#समावेश "kernel/system.h"
#समावेश <unistd.h>

#अगर USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
पूर्णांक do_abort(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  पूर्णांक how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  वापस(OK);				/* pro-forma (really EDISASTER) */
}

#पूर्ण_अगर /* USE_ABORT */

