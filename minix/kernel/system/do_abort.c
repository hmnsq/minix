/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <unistd.h>

#यदि USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
पूर्णाङ्क do_abort(संरचना proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  पूर्णाङ्क how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  फर्काऊ(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

