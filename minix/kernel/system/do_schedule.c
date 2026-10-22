#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ចំនួនគត់ do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	ចំនួនគត់ proc_nr;
	ចំនួនគត់ priority, quantum, cpu, niced;

	ប្រសិនបើ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ត្រឡប់ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ប្រសិនបើ (caller != p->p_scheduler)
		ត្រឡប់(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ត្រឡប់ sched_proc(p, priority, quantum, cpu, niced);
}
