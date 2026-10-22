/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#समावेश "kernel/system.h"

#समावेश <signal.h>

#अगर USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
पूर्णांक do_exit(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  पूर्णांक sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  वापस(EDONTREPLY);			/* don't reply */
}

#पूर्ण_अगर /* USE_EXIT */

