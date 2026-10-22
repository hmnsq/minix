#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ཧྲིལ do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	ཧྲིལ proc_nr;
	ཧྲིལ priority, quantum, cpu, niced;

	གལ་ཏེ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ཕྱིར EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	གལ་ཏེ (caller != p->p_scheduler)
		ཕྱིར(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ཕྱིར sched_proc(p, priority, quantum, cpu, niced);
}
