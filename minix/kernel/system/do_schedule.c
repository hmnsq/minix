#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
puur_naa_nka do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	puur_naa_nka proc_nr;
	puur_naa_nka priority, quantum, cpu, niced;

	ᰀᰧ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ᰀᰧᰌ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ᰀᰧ (caller != p->p_scheduler)
		ᰀᰧᰌ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ᰀᰧᰌ sched_proc(p, priority, quantum, cpu, niced);
}
