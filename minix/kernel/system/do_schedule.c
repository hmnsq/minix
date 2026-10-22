#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
පූර්ණ_සංඛ්‍යා do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	පූර්ණ_සංඛ්‍යා proc_nr;
	පූර්ණ_සංඛ්‍යා priority, quantum, cpu, niced;

	නම් (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ආපසු EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	නම් (caller != p->p_scheduler)
		ආපසු(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ආපසු sched_proc(p, priority, quantum, cpu, niced);
}
