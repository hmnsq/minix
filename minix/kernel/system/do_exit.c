/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಆದರೆ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_exit(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ಪೂರ್ಣಾಂಕ sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ಹಿಂತಿರುಗಿ(EDONTREPLY);			/* don't reply */
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_EXIT */

