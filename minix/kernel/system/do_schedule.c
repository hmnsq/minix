#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
पूर्णांक do_schedule(संरचना proc * caller, message * m_ptr)
{
	संरचना proc *p;
	पूर्णांक proc_nr;
	पूर्णांक priority, quantum, cpu, niced;

	जर (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		परत EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	जर (caller != p->p_scheduler)
		परत(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	परत sched_proc(p, priority, quantum, cpu, niced);
}
