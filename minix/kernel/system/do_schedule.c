#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	ຈຳນວນເຕັມ proc_nr;
	ຈຳນວນເຕັມ priority, quantum, cpu, niced;

	ຖ້າ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ສົ່ງຄືນ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ຖ້າ (caller != p->p_scheduler)
		ສົ່ງຄືນ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ສົ່ງຄືນ sched_proc(p, priority, quantum, cpu, niced);
}
