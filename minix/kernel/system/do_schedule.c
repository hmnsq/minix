#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
целое do_schedule(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	целое proc_nr;
	целое priority, quantum, cpu, niced;

	если (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		вернуть EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	если (caller != p->p_scheduler)
		вернуть(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	вернуть sched_proc(p, priority, quantum, cpu, niced);
}
