/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <unistd.h>

#ఒకవేళ USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
పూర్ణాంకం do_abort(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  పూర్ణాంకం how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  తిరిగి(OK);				/* pro-forma (really EDISASTER) */
}

#అయితే_ముగింపు /* USE_ABORT */

