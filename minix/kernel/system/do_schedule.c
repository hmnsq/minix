#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
цео do_schedule(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	цео proc_nr;
	цео priority, quantum, cpu, niced;

	ако (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		врати EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ако (caller != p->p_scheduler)
		врати(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	врати sched_proc(p, priority, quantum, cpu, niced);
}
