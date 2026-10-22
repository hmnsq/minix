#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ціле do_schedule(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	ціле proc_nr;
	ціле priority, quantum, cpu, niced;

	якщо (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		повернути EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	якщо (caller != p->p_scheduler)
		повернути(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	повернути sched_proc(p, priority, quantum, cpu, niced);
}
