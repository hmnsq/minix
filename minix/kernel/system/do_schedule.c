#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
पूर्णाङ्क do_schedule(संरचना proc * caller, message * m_ptr)
{
	संरचना proc *p;
	पूर्णाङ्क proc_nr;
	पूर्णाङ्क priority, quantum, cpu, niced;

	सचे (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		पटिनिवत्त EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	सचे (caller != p->p_scheduler)
		पटिनिवत्त(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	पटिनिवत्त sched_proc(p, priority, quantum, cpu, niced);
}
