#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>
#অন্তর্ভাব "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_schedule(সংরচনা proc * caller, message * m_ptr)
{
	সংরচনা proc *p;
	পূর্ণাঙ্ক proc_nr;
	পূর্ণাঙ্ক priority, quantum, cpu, niced;

	যদি (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		প্রত্যাগম EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	যদি (caller != p->p_scheduler)
		প্রত্যাগম(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	প্রত্যাগম sched_proc(p, priority, quantum, cpu, niced);
}
