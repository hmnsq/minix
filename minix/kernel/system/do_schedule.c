#include "kernel/system.h"
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ကိန်းပြည့် do_schedule(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	ကိန်းပြည့် proc_nr;
	ကိန်းပြည့် priority, quantum, cpu, niced;

	အကယ်၍ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ပြန်ပေး EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	အကယ်၍ (caller != p->p_scheduler)
		ပြန်ပေး(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ပြန်ပေး sched_proc(p, priority, quantum, cpu, niced);
}
