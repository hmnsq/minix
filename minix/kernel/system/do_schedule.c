#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>
#ضمن "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحيح do_schedule(بنية proc * caller, message * m_ptr)
{
	بنية proc *p;
	صحيح proc_nr;
	صحيح priority, quantum, cpu, niced;

	إذا (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		إرجاع EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	إذا (caller != p->p_scheduler)
		إرجاع(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	إرجاع sched_proc(p, priority, quantum, cpu, niced);
}
