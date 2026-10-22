/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಆದರೆ USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_abort(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
 */
  ಪೂರ್ಣಾಂಕ how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  ಹಿಂತಿರುಗಿ(OK);				/* pro-forma (really EDISASTER) */
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_ABORT */

