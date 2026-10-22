/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <unistd.h>

#যদি USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_abort(কাঠামো proc * caller, message * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  পূর্ণসংখ্যা how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  ফেরত(OK);				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

