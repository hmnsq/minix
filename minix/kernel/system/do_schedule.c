#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
整数 do_schedule(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	整数 proc_nr;
	整数 priority, quantum, cpu, niced;

	如果 (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		返回 EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	如果 (caller != p->p_scheduler)
		返回(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	返回 sched_proc(p, priority, quantum, cpu, niced);
}
