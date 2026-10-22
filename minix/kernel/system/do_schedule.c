#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/endpoint.h>
#அந்தர்பாவ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
முழு do_schedule(கட்டமைப்பு proc * caller, message * m_ptr)
{
	கட்டமைப்பு proc *p;
	முழு proc_nr;
	முழு priority, quantum, cpu, niced;

	எனில் (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		திரும்பு EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	எனில் (caller != p->p_scheduler)
		திரும்பு(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	திரும்பு sched_proc(p, priority, quantum, cpu, niced);
}
