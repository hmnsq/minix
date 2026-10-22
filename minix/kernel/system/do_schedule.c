#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/endpoint.h>
#অন্তর্ভাৱ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_schedule(সংরচনা proc * caller, message * m_ptr)
{
	সংরচনা proc *p;
	পূর্ণাঙ্ক proc_nr;
	পূর্ণাঙ্ক priority, quantum, cpu, niced;

	যদি (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ঘূৰাই EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	যদি (caller != p->p_scheduler)
		ঘূৰাই(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ঘূৰাই sched_proc(p, priority, quantum, cpu, niced);
}
