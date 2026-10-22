#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>
#অন্তর্ভাব "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_schedule(কাঠামো proc * caller, message * m_ptr)
{
	কাঠামো proc *p;
	পূর্ণসংখ্যা proc_nr;
	পূর্ণসংখ্যা priority, quantum, cpu, niced;

	যদি (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ফেরত EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	যদি (caller != p->p_scheduler)
		ফেরত(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ফেরত sched_proc(p, priority, quantum, cpu, niced);
}
