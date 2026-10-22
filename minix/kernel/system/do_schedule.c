#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
정수 do_schedule(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	정수 proc_nr;
	정수 priority, quantum, cpu, niced;

	만약 (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		반환 EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	만약 (caller != p->p_scheduler)
		반환(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	반환 sched_proc(p, priority, quantum, cpu, niced);
}
