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

	ܐܢ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ܦܢܐ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ܐܢ (caller != p->p_scheduler)
		ܦܢܐ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ܦܢܐ sched_proc(p, priority, quantum, cpu, niced);
}
