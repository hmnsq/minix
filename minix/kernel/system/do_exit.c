/* The kernel call implemented in this file:
 *   m_type:	SYS_EXIT
 */

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <signal.h>

#ఒకవేళ USE_EXIT

/*===========================================================================*
 *				 do_exit				     *
 *===========================================================================*/
పూర్ణాంకం do_exit(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_exit. A system process has requested to exit. Generate a
 * self-termination signal.
 */
  పూర్ణాంకం sig_nr = SIGABRT;

  cause_sig(caller->p_nr, sig_nr);      /* send a signal to the caller */

  తిరిగి(EDONTREPLY);			/* don't reply */
}

#అయితే_ముగింపు /* USE_EXIT */

