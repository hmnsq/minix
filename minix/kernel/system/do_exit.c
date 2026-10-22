/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

#ଅନ୍ତର୍ଭାଵ <signal.h>

#ଯଦି USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_exit(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  ଫେରନ୍ତୁ(EDONTREPLY);			/* don't reply */
}

#endif /* USE_EXIT */

