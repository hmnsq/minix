#شامل "kernel/system.h"
#شامل <minix/endpoint.h>
#شامل "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحیح do_schedule(ساختار proc * caller, message * m_ptr)
{
	ساختار proc *p;
	صحیح proc_nr;
	صحیح priority, quantum, cpu, niced;

	اگر (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		بازگشت EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	اگر (caller != p->p_scheduler)
		بازگشت(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	بازگشت sched_proc(p, priority, quantum, cpu, niced);
}
