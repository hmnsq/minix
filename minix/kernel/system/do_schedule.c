#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/endpoint.h>
#અન્તર્ભાવ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
પૂર્ણાંક do_schedule(સંરચના proc * caller, message * m_ptr)
{
	સંરચના proc *p;
	પૂર્ણાંક proc_nr;
	પૂર્ણાંક priority, quantum, cpu, niced;

	જો (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		પાછા EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	જો (caller != p->p_scheduler)
		પાછા(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	પાછા sched_proc(p, priority, quantum, cpu, niced);
}
