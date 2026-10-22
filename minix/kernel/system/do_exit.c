/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಯದಿ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_exit(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ಪೂರ್ಣಾಙ್ಕ sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ಪ್ರತ್ಯಾಗಮ(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

