#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
จำนวนเต็ม do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	จำนวนเต็ม proc_nr;
	จำนวนเต็ม priority, quantum, cpu, niced;

	ถ้า (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		คืน EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ถ้า (caller != p->p_scheduler)
		คืน(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	คืน sched_proc(p, priority, quantum, cpu, niced);
}
