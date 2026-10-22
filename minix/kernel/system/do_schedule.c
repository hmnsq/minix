#समावेश "kernel/system.h"
#समावेश <minix/endpoint.h>
#समावेश "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
पूर्णांक do_schedule(काष्ठा proc * caller, message * m_ptr)
{
	काष्ठा proc *p;
	पूर्णांक proc_nr;
	पूर्णांक priority, quantum, cpu, niced;

	अगर (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		वापस EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	अगर (caller != p->p_scheduler)
		वापस(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	वापस sched_proc(p, priority, quantum, cpu, niced);
}
